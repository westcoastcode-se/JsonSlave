# JsonSlave

This application is a very simple WebServer that understands a very small subset of the HTTP standard.

Please note that **THIS SERVER IS NOT SUPPOSED TO RUN IN ANY PRODUCTION ENVIRONMENT**. The purpose for this application
is to make it easy to do basic JSON methods in a small and easy-to-start application.

All content is stored in the `data` folder. This folder is placed in the same directory as where the executable is
running from. The server itself is exposed over port `8080` and will accept incoming connections from any IP address.

It will allow any form of URL, but the path will be transformed by changing all special characters into `+`. For 
example, it will convert `/players/guild/awesomeo/` into `players+guild+awesomeo.json`. The file itself will always
be placed in the root of the data folder.

## Usage

### GET

You can fetch a stored json entry by calling `localhost:8080` (or using your computer's IP address). The path after that
is then used as basis for where the object is stored on your machine.

For example

```bash
curl localhost:8080/players/existing_user
```

Will return the content of the file `data/players+existing_user.json`.

You can manually create or update files by creating files following this pattern.

If you request JSON content that does not exist, then a **404** will be returned:

```bash
curl localhost:8080/players/dont_exist -D -
HTTP/1.1 404 Not Found
Server: JsonSlave 1.0.0
Connection: Closed
Content-Type: application/json
Content-Length: 0
```

### PUT

You can also add and update json entries by sending a PUT to the server. For example:

```bash
curl http://127.0.0.1:8080/players/john_doe -XPUT -d '{"name":"John Doe"}'
```

This will add a file called `players+john_doe.json` in the data folder with the content `{"name":"John Doe"}`.

If the item already exist, then the previous value will be returned to the client

### DELETE

You can also delete json entries by sending a DELETE request to the server. For example:

```bash
curl http://127.0.0.1:8080/players/john_doe -XPUT -d '{"name":"John Doe"}'
```

If the JSON entry exist then a **200** is returned with the removed content. Otherwise a **404** will be returned.

The file itself will not be deleted. It will, instead, be renamed from `players+john_doe.json` into 
`players+john_doe.json.removed`. This is so that we can manually undo the delete if we want to.