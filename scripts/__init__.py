import logging

from .consts import *
from .utils import *

logging.basicConfig(
    level=logging.DEBUG,
    format="[%(filename)18s:%(lineno)-3d] %(message)s",
)
