#include <iostream>
#include "Network/TcpSocket.hpp"
#include "Http/HttpHeaders.hpp"
#include "Store/DiskJsonStorage.hpp"
#include "Utils/ThreadPool.hpp"

namespace
{
	atomic_bool running(true);
	IJsonStorage* storage;
}

void SendResponse(shared_ptr<TcpSocketStream> stream, int statusCode, const string& data) {
	stream->WriteLineFormatted("HTTP/1.1 %d %s", statusCode, HttpStatus::CodeToString(statusCode).c_str());
	stream->WriteLine("Server: JsonSlave " SERVER_VERSION);
	stream->WriteLine("Connection: Closed");
	stream->WriteLine("Content-Type: application/json");
	stream->WriteLineFormatted("Content-Length: %d", data.length());
	stream->WriteLine();

	if (!data.empty()) {
		stream->Write(data);
	}
}

void DoGet(const HttpHeaders* headers, shared_ptr<TcpSocketStream> stream) {
	const string result = storage->Get(headers->GetPath());
	if (result.empty())
		SendResponse(stream, 404, result);
	else
		SendResponse(stream, 200, result);
}

void DoPut(const HttpHeaders* headers, shared_ptr<TcpSocketStream> stream) {
	const auto header = headers->FindHeader(HttpHeaders::ContentLength);
	if (header == nullptr) {
		throw storage_error("Failed to store content. Expected a Content-Length request header");
	}

	const auto newValue = stream->Read(header->GetIntValue());
	const auto oldValue = storage->Add(headers->GetPath(), newValue);
	SendResponse(stream, 200, oldValue);
}

void DoDelete(const HttpHeaders* headers, shared_ptr<TcpSocketStream> stream) {
	const string oldValue = storage->Remove(headers->GetPath());
	if (oldValue.empty())
		SendResponse(stream, 404, oldValue);
	else
		SendResponse(stream, 200, oldValue);
}

void ClientThread(TcpSocket* socket) {
	// Open a stream and fetch the request's headers
	auto stream = socket->OpenStream();

	try {
		auto headers = HttpHeaders::FromStream(stream);

		// Log the incoming request
		cout << socket->GetAddress() << " " << headers->GetMethod() << " " << headers->GetPath() << endl;

		// Process the incoming request
		const string& method = headers->GetMethod();
		if (method == HttpMethods::Get) {
			DoGet(headers.get(), stream);
		} else if (method == HttpMethods::Put) {
			DoPut(headers.get(), stream);
		} else if (method == HttpMethods::Delete) {
			DoDelete(headers.get(), stream);
		}
	} catch (storage_error& e) {
		stringstream ss;
		ss << R"({"message":")" << e.what() << "\"}";
		SendResponse(stream, 500, ss.str());
	} catch (runtime_error& e) {
		cout << "[ERROR] Unhandled error: " << e.what() << endl;
	}
	delete socket;
}

void StopServer(int s) {
	running = false;
	cout << "Shutting down the server" << endl;
}

int main(int argc, char** argv) {
	signal(SIGINT, StopServer);

	// Create a storage
	storage = new DiskJsonStorage("data");

	// Create thread pool with 32 worker threads
	ThreadPool pool(32);

	// Start listen for incoming connections
	try {
		auto socket = TcpSocket::Listen(8080);
		cout << "Start listening on port " << 8080 << endl;
		while (running) {
			TcpSocket* client = socket->Accept();
			if (client != nullptr) {
				pool.Execute([client]() { ClientThread(client); });
			}
		}
		//pool.Stop();
		delete socket;
	} catch (runtime_error& e) {
		cout << e.what() << endl;
	}

	cout << "Removing storage" << endl;
	delete storage;
	storage = nullptr;
	cout << "Storage removed" << endl;
	return 0;
}
