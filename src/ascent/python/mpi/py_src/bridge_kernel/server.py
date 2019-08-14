# Copyright 2019 Lawrence Livermore National Security, LLC and other
# Bridge Kernel Project Developers. See the top-level LICENSE file for details.
#
# SPDX-License-Identifier: BSD-3-Clause

import ascent_extract
import conduit
import sys

import time

import os
import socket

import json
import numpy

try:
    from urllib.request import urlopen
except ImportError:
    from urllib2 import urlopen

try:
    from base64 import encodebytes
except ImportError:
    from base64 import encodestring as encodebytes

from .mpi_server import MPIServer
from . import utils


global_dict = {}

global_dict["MPI_ENABLED"] = hasattr(ascent_extract, 'ascent_mpi_comm_id')

if global_dict["MPI_ENABLED"]:
  import ascent.mpi
  from mpi4py import MPI
else:
  import ascent

def jupyter_extract():
    global global_dict

    #The if branch runs once per simulation, else runs for each given timestep
    if "ascent_server" not in global_dict:
        ascent_opts = conduit.Node()
        if global_dict["MPI_ENABLED"]:
          comm = MPI.Comm.f2py(ascent_extract.ascent_mpi_comm_id())
          ascent_opts["mpi_comm"].set(comm.py2f())
          server_ascent = ascent.mpi.Ascent()
        else:
          server_ascent = ascent.Ascent()

        ascent_opts["actions_file"] = ""
        server_ascent.open(ascent_opts)

        global_dict["server_ascent"] = server_ascent
    else:
        #TODO is this needed here?
        if global_dict["MPI_ENABLED"]:
          comm = MPI.Comm.f2py(ascent_extract.ascent_mpi_comm_id())

    global_dict["server_ascent"].publish(ascent_extract.ascent_data())

    def display_images(info):
        for img in info["images"].children():
            with open(img.node()["image_name"], 'rb') as f:
                img_content = f.read()
                #TODO this doesn't work
                image(img_content, "png")

    def get_encoded_images(info):
        images = []
        for img in info["images"].children():
            with open(img.node()["image_name"], 'rb') as f:
                images.append(encodebytes(f.read()).decode('utf-8'))
        return images

    def run_transformation(transformation, info, *args, **kwargs):
        render = None
        for child in info["actions"].children():
          if child.node()["action"] == "add_scenes":
            #get the first render
            render = child.node()["scenes"][0]["renders"][0]
            break
        if render is None:
          print("ERROR: no scenes found")
        #keeps the defaults from info['actions']
        render.update(transformation(info, *args, **kwargs))
        #TODO this is temporary
        render["image_name"] = "out_ascent_render_3d"
        return info["actions"]

    #TODO store MPIServer in global variable instead of passing it
    #TODO why doesn't if rank==0 at the top work?
    def handle_message(server, message):
        with server._redirect():
            #TODO move this up?
            server_ascent = global_dict['server_ascent']

            info = conduit.Node()
            server_ascent.info(info)

            data = message["code"]

            if data["type"] == "transform":
                call_obj = data["code"]

                #TODO better way to do this so i don't have to import everything into this file
                #eval args
                args = []
                for arg in call_obj["args"]:
                    args.append(eval(arg))

                #TODO eval kwargs
                kwargs = call_obj["kwargs"]

                actions = run_transformation(utils.utils_dict[call_obj["name"]], info, *args, *kwargs)
                server_ascent.execute(actions)

                server.root_writemsg({"type": "transform"})
            elif data["type"] == "get_images":
                server.root_writemsg({
                    "type": "images",
                    "code": get_encoded_images(info),
                })
            elif data["type"] == "get_ascent_info":
                server.root_writemsg({
                        "type": "ascent_info",
                        "code": info.to_json(),
                    })
            elif data["type"] == "next":
                server_ascent.execute(info['actions'])

                server.root_writemsg({"type": "next"})

    my_prefix = "%s_%s" % (os.path.splitext(os.path.basename(sys.argv[0]))[0], socket.gethostname())

    my_ns = {
      'display_images': display_images,
      'conduit': conduit,
      'ascent_data': ascent_extract.ascent_data,
      'server_ascent': global_dict['server_ascent'],
    }
    if global_dict["MPI_ENABLED"]:
      my_ns['MPI'] = MPI
      my_ns['ascent_mpi_comm_id'] = ascent_extract.ascent_mpi_comm_id
      MPIServer(comm=comm, ns=my_ns, callback=handle_message, prefix=my_prefix).serve()
    else:
      MPIServer(comm=None, ns=my_ns, callback=handle_message, prefix=my_prefix).serve()