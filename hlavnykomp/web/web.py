#!/usr/bin/env python
# -*- coding: utf8 -*-

# Quick start:
#
# virtualenv venv
# source venv/bin/activate
# pip install flask markdown
# ./web.py

import os
import re
import time
import tarfile
from flask import (abort, flash, Flask, make_response, redirect, url_for,
    render_template, render_template_string, request, send_file, session)
from jinja2 import Markup
from hashlib import sha1
from markdown import markdown
import random

app = Flask(__name__)
app.config.from_pyfile('../webconfig')
if os.getenv('FLASK_DEBUG'): app.debug = False

if not app.debug:
    import sys
    import logging
    handler = logging.StreamHandler(sys.stderr)
    handler.setLevel(logging.WARNING)
    app.logger.addHandler(handler)

druzinky = app.config['DRUZINKY']
passwords = dict((id, hash) for id, title, hash in druzinky)
usertitles = dict((id, title) for id, title, hash in druzinky)
for id, title, hash in druzinky:
    if not isinstance(id, str) or not id.isalpha():
        raise ValueError('%r not alpha' % id)


@app.context_processor
def template_vars():
    return dict(druzinky=druzinky, usertitles=usertitles, username=session.get('login'))


def get_records():
    records = {}
    for basename in os.listdir('../../zaznamy/'):
        path = '../../zaznamy/'+basename
        if basename.startswith('.'): continue
        if not basename.endswith('.manifest'): continue
        if not os.path.isfile(path): continue
        id = basename[:-len('.manifest')]
        lines = open(path).readlines()
        manifest = dict(line.rstrip('\n').split('=', 1) for line in lines)
        records[id] = manifest
    return records


def get_ranklist():
    ranks = dict((id, 0.0) for id, title, hash in druzinky)
    all_records = sorted(get_records().items())
    for id, manifest in all_records:
        if manifest['state'] != 'displayed': continue
        builds = manifest['clients'].strip(',').split(',')
        myranks = manifest['rank'].strip(',').split(',')
        for i, build in enumerate(builds):
            client = build.partition('/')[0]
            ranks[client] = (ranks[client] + int(myranks[i]))# * 0.97
    return reversed(sorted(((round(rank, 2), id) for id, rank in ranks.items())))


def get_uploads(username):
    if not username.isalpha(): return []
    uploads = []
    
    upload_path = '../uploady/'+username
    if not os.path.exists(upload_path):
        os.makedirs(upload_path)
    
    for basename in sorted(os.listdir(upload_path), reverse=True):
        if not basename.endswith('.tar.gz'): continue
        id = basename[:-len('.tar.gz')]
        builddir = '../buildy/'+username+'/'+id
        if os.path.isfile(builddir+'/.status.ok'):
            uploads.append((id, True))
        elif os.path.isfile(builddir+'/.status.err'):
            uploads.append((id, False))
        else:
            uploads.append((id, None))
    return uploads


@app.route("/")
def index(all_uploads=False):
    ranklist = Markup(render_template('ranklist.html', ranklist=get_ranklist()))
    uploads = None
    uploads_max = None
    if 'login' in session:
        uploads = get_uploads(session['login'])
        uploads_max = len(uploads) if all_uploads else 5
    return render_template('index.html', ranklist=ranklist,
        uploads=uploads, uploads_max=uploads_max)


@app.route("/ranklist")
def rankpage():
    # urcene na pustenie na hlavnom kompe v browseri apod
    ranklist = Markup(render_template('ranklist.html', ranklist=get_ranklist()))
    response_string = render_template_string('''
        {% extends "layout.html" %}
        {% block body %}{{ ranklist }}{% endblock %}
        ''', ranklist=ranklist)
    response = make_response(response_string)
    response.headers['Refresh'] = '5'
    return response


@app.route("/login", methods=['POST'])
def login():
    username = request.form['druzinka']
    password = request.form['heslo']
    hash = sha1('abc'+password+'def').hexdigest()
    if passwords.get(username) == hash:
        session.clear()
        session['login'] = username
        return redirect(url_for('index'))
    flash(u'Nepodarilo sa prihlásiť. Zlá družinka alebo heslo.', 'error')
    return redirect(url_for('index'))


@app.route("/logout", methods=['POST'])
def logout():
    session.clear()
    return redirect(url_for('index'))


@app.route("/upload", methods=['POST'])
def upload():
    if 'login' not in session: abort(403)
    if 'archiv' not in request.files:
        flash(u'Nebol vybratý žiaden súbor.', 'error')
        return redirect(url_for('index'))
    try:
        with tarfile.open(fileobj=request.files['archiv']) as tar:
            names = tar.getnames()
        request.files['archiv'].seek(0)
    except Exception:
        flash(Markup(u'Musí to byť <code>tar.gz</code> alebo '
                     u'<code>tgz</code>.'), 'error')
        return redirect(url_for('index'))
    if 'makefile' not in names and 'Makefile' not in names:
        flash(Markup(u'V koreni archívu musí byť '
                     u'súbor <code>Makefile</code>.'), 'error')
        return redirect(url_for('index'))
    dir = '../uploady/'+session['login']+'/'
    basename = time.strftime('%Y-%m-%d-%H-%M-%S')+"_%s_"%random.randint(0,100000)+".tar.gz"
    request.files['archiv'].save(dir + basename)
    flash(Markup(u'Klient úspešne uložený ako <code>%s</code>.' % basename),
          'success')
    return redirect(url_for('index'))


@app.route("/uploads")
def uploads():
    if 'login' not in session: abort(403)
    return index(all_uploads=True)


@app.route("/uploads/<id>")
def uploads_download(id):
    if 'login' not in session: abort(403)
    if not re.match(r'^[-0-9_]+\.tar\.gz$', id): abort(404)
    path = '../uploady/'+session['login']+'/'+id
    if not os.path.isfile(path): abort(404)
    return send_file(path, as_attachment=True)


@app.route("/uploads/<id>/status")
def uploads_status(id):
    if 'login' not in session: abort(403)
    if not re.match(r'^[-0-9]+$', id): abort(404)
    builddir = '../buildy/'+session['login']+'/'+id+'/'
    for basename in ['.status.ok', '.status.err']:
        if os.path.isfile(builddir+basename):
            return send_file(builddir+basename, mimetype='text/plain')
    abort(404)


@app.route("/records")
def records():
    state_captions = {
        'displayed': None,
        'playing': u'(ešte beží server)',
        'displaying': u'(čaká na ukázanie)',
        'crashed': Markup(u'<strong>spadol server</strong>'),
    }
    data = []
    for id, manifest in sorted(get_records().items()):
        begin = time.strftime('%a %H:%M:%S', time.localtime(
            int(manifest['begin'])))
        path = '../../zaznamy/'+id+'.tar.gz'
        pathg= '../../zaznamy/'+id+'.png'
        link = None
        linkg = None
        if manifest['state'] == 'displayed' and os.path.isfile(path):
            link = url_for('records_download', id=id+'.tar.gz')
        if os.path.isfile(pathg):
            linkg = url_for('graphs_download', id=id+'.png')
        mapa = manifest['map'].replace('mapy/', '')
        state = state_captions[manifest['state']]
        rank = None
        if manifest['state'] == 'displayed':
            rank = [int(r) for r in manifest['rank'].split(',')]
        clients = map(lambda qq: qq.split('/')[0], manifest['clients'].strip(',').split(','))
        if rank:
            rank = zip(clients,rank)
            rank = sorted(rank)
        data.append(dict(id=id, begin=begin, link=link, linkg=linkg, map=mapa, state=state,
            rank=rank))
    return render_template('records.html', records=data)


@app.route("/records/<id>")
def records_download(id):
    if not re.match(r'^[0-9]+\.tar\.gz$', id): abort(404)
    id = id[:-len('.tar.gz')]
    records = get_records()
    if id not in records: abort(404)
    if records[id]['state'] != 'displayed': abort(403)
    path = '../../zaznamy/'+id+'.tar.gz'
    if not os.path.isfile(path): abort(404)
    return send_file(path, as_attachment=True)

@app.route("/graphs/<id>")
def graphs_download(id):
    if not re.match(r'^[0-9]+\.png$', id): abort(404)
    id = id[:-len('.png')]
    #records = get_records()
    #if id not in records: abort(404)
    #if records[id]['state'] != 'displayed': abort(403)
    path = '../../zaznamy/'+id+'.png'
    if not os.path.isfile(path): abort(404)
    return send_file(path, as_attachment=True)

@app.route("/tar")
def tar_download():
    path = '../../proboj.tar.gz'
    if not os.path.isfile(path): abort(404)
    return send_file(path, as_attachment=True)

@app.route("/mapy")
def mapy_lister():
    maps = []
    for mapname in os.listdir('../../mapy/'):
      maps.append(mapname)
    
    html = ""
    for mapa in maps:
        html+='<a href="/mapy/%s">%s</a><br>'%(mapa,mapa)
    return html

@app.route("/mapy/<id>")
def serve_map(id):
    mapbase = '../../mapy/'
    
    for mapname in os.listdir(mapbase):
        if  id==mapname:
            path = mapbase+id
            return send_file(path, as_attachment=True)
            
    if not os.path.isfile(path): abort(404)


@app.route("/docs")
def docs():
    with open('../../dokumentacia.md') as fh:
        content = Markup(markdown(unicode(fh.read(), 'utf-8')))
    return render_template('doc.html', content=content)


if __name__ == '__main__':
    app.run(host=('0.0.0.0' if os.getenv('FLASK_PUBLIC') else '127.0.0.1'), threaded=True)

