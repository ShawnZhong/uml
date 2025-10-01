import logging

from .utils import *
from .consts import *
from .linux import *

logging.basicConfig(
    level=logging.DEBUG,
    format="[%(filename)18s:%(lineno)-3d] %(message)s",
)
