import sys
from pathlib import Path

# Add parent directory to path so we can import the main app
sys.path.insert(0, str(Path(__file__).parent.parent))

from app import app

async def handler(event, context):
    """Netlify Functions handler for Flask app"""
    from flask import Flask
    from serverless_wsgi import ServerlessWSGIMiddleware
    
    if isinstance(app, Flask):
        app.wsgi_app = ServerlessWSGIMiddleware(app.wsgi_app)
    
    return app(event, context)