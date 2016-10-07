## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

import os
import Options
import os.path
import ns3waf
import sys
import types
# local modules
import wutils
import subprocess
import Logs
from waflib.Errors import WafError

def configure(conf):
    ns3waf.check_modules(conf, ['core', 'network', 'dce', 'wifi', 'point-to-point', 'csma', 'mobility'], mandatory = True)

def build(bld):
    if bld.env['KERNEL_STACK']:
      bld.build_a_script('dce', needed = ['core', 'network', 'dce', 'wifi', 'point-to-point', 'csma', 'mobility'],
				  target='bin/kupakupa',

				  source=['kupakupa.cc', 'tinystr.cpp','tinyxml.cpp','tinyxmlerror.cpp','tinyxmlparser.cpp','parseXml.cpp'],
#				  linkflags=['-L/usr/local/lib'],
#				  lib=['foolib']
				  )

