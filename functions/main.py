import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent))

from app import app
import serverless_wsgi


def handler(event, context):
    """Netlify Functions handler for Flask app."""
    return serverless_wsgi.handle_request(app, event, context)
