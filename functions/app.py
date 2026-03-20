import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent))

from app import app
from serverless_wsgi import ServerlessWSGIMiddleware

# Wrap the Flask app with WSGI middleware once when the function loads
wsgi_handler = ServerlessWSGIMiddleware(app)

def handler(event, context):
    """Netlify Functions handler for Flask app"""
    return wsgi_handler(event, context)