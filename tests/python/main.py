from http.server import HTTPServer, BaseHTTPRequestHandler
import base64
import gearbox_test

from server import Session

HOST = 'localhost'
PORT = 9999

SERVER_NAME = 'Transmission'
CONTENT_TYPE = 'application/json'
RPC_PATH = '/transmission/rpc'

USERNAME = 'username'
PASSWORD = 'password'

BAD_AUTH_CODE = 401
BAD_AUTH_HEADER_KEY = 'WWW-Authenticate'
BAD_AUTH_HEADER_VAL = 'Basic realm="{}"'.format(SERVER_NAME)
BAD_AUTH_CONTENT_TYPE = 'text/html; charset=ISO-8859-1'
BAD_AUTH_DATA = '<h1>401: Unauthorized</h1>Unauthorized User'

BAD_REQUEST_CODE = 405
BAD_REQUEST_CONTENT_TYPE = 'text/html; charset=ISO-8859-1'
BAD_REQUEST_DATA = '<h1>405: Method Not Allowed</h1>'

CONNECT_TEST_PATH = '/test_connection'
CONNECT_TEST_CODE = 200
CONNECT_TEST_CONTENT_TYPE = 'text/plain'
CONNECT_TEST_DATA = 'OK'

def make_response_bad_auth():
    response = Response()
    response.code = BAD_AUTH_CODE
    response.headers[BAD_AUTH_HEADER_KEY] = BAD_AUTH_HEADER_VAL
    response.content_type = BAD_AUTH_CONTENT_TYPE
    response.data = BAD_AUTH_DATA
    return response

def make_response_bad_request():
    response = Response()
    response.code = BAD_REQUEST_CODE
    response.content_type = BAD_REQUEST_CONTENT_TYPE
    response.data = BAD_REQUEST_DATA
    return response

def make_response_test_connection():
    response = Response()
    response.code = CONNECT_TEST_CODE
    response.content_type = CONNECT_TEST_CONTENT_TYPE
    response.data = CONNECT_TEST_DATA
    return response

class Request:
    def __init__(self):
        self.headers = { }
        self.data = ''

class Response:
    def __init__(self):
        self.code = 200
        self.headers = { } 
        self.content_type = CONTENT_TYPE
        self.data = ''

    def send(self, request_handler):
        request_handler.send_response(self.code)

        for key, value in self.headers.items():
            request_handler.send_header(key, value)
        request_handler.send_header('Content-Length', len(self.data))
        request_handler.send_header('Content-Type', self.content_type)
        request_handler.end_headers()

        request_handler.wfile.write(self.data.encode('utf-8'))

class HTTPRequestHandler(BaseHTTPRequestHandler):
    def prepare(self):
        self.protocol_version = 'HTTP/1.1'
        self.server_version = SERVER_NAME
        self.sys_version = ''

    def address_string(self):
        host, port = self.client_address[:2]
        self.address_string = host
        return host

    def is_authenticated(self):
        result = False

        header_value = self.headers['Authorization']
        if header_value and header_value.startswith('Basic ') :
            auth_data = base64.b64decode(header_value.split()[1]).decode('utf-8')
            result = '{}:{}'.format(USERNAME, PASSWORD) == auth_data

        return result

    def do_GET(self):
        self.prepare()

        response = Response()

        if self.path == CONNECT_TEST_PATH:
            response = make_response_test_connection()
            response.data += ' GET'
        else:
            if not self.is_authenticated():
                response = make_response_bad_auth()
            else:
                response = make_response_bad_request()

        response.send(self)

    def do_POST(self):
        self.prepare()

        request = Request()
        request.headers = self.headers
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length)
        request.data = post_data.decode('utf-8')
        response = Response()

        if self.path == CONNECT_TEST_PATH:
            response = make_response_test_connection()
            response.data += ' ' + request.data
        else:
            if not self.is_authenticated():
                response = make_response_bad_auth()
            else:
                if not self.path == RPC_PATH:
                    response = make_response_bad_request()
                else:
                    Session.handle_request(request, response)

        response.send(self)

if __name__ == "__main__":
    httpd = HTTPServer((HOST, PORT), HTTPRequestHandler)
    gearbox_test.server_ready()
    httpd.serve_forever()
