import os
import json

from switch import Switch

session_id = 'dGVzdGhlZHNnZGpraGtkamhha2ZjZ3dlODN3aXVkc2hhZm4n'

stats = None
torrents = None
recently_active = None

with open(os.path.join(os.path.dirname(__file__), 'json/stats.json')) as stats_json:
    stats = json.load(stats_json)
def get_statistics(request):
    stats['tag'] = request['tag']
    return json.dumps(stats) + '\n'

with open(os.path.join(os.path.dirname(__file__), 'json/torrents.json')) as torrents_json:
    torrents = json.load(torrents_json)
with open(os.path.join(os.path.dirname(__file__), 'json/recently_active.json')) as recently_active_json:
    recently_active = json.load(recently_active_json)
def get_torrents(request):
    if 'ids' in request['arguments'] and request['arguments']['ids'] == 'recently-active':
        recently_active['tag'] = request['tag']
        return json.dumps(recently_active) + '\n'
    else:
        torrents['tag'] = request['tag']
        return json.dumps(torrents) + '\n'

def test_send_request(request):
    result = {
        'arguments': { 'args': 0 },
        'result': 'success',
        'tag': request['tag']
    }
    return json.dumps(result) + '\n';

class Session:
    @staticmethod
    def handle_request(request, response):
        request_data = None
        try:
            request_data = json.loads(request.data)
        except:
            None

        response.headers['X-Transmission-Session-Id'] = session_id

        if 'X-Transmission-Session-Id' not in request.headers or \
                        request.headers['X-Transmission-Session-Id'] != session_id:
            response.code = 409
            response.content_type = 'text/html; charset=ISO-8859-1'
            response.data = '<h1>Bad X-Transmission-Session-Id</h1>'
        else:
            if request_data:
                if 'method'    not in request_data or \
                   'arguments' not in request_data or \
                   'tag'       not in request_data:
                    response.code = 405
                    response.data = '<h1>405: Method Not Allowed</h1>'
                else:
                    for case in Switch(request_data['method']):
                        if case('session-stats'):
                            response.data = get_statistics(request_data)
                            break
                        if case('torrent-get'):
                            response.data = get_torrents(request_data)
                            break
                        if case('test_send_request'):
                            response.data = test_send_request(request_data)
                            break
                        if case():
                            response.data = json.dumps({'arguments':{}, 'result':'uninmplemented'}) + '\n'
                            break
            else:
                response.data = json.dumps({'arguments':{},'result':'no method name'}) + '\n'
