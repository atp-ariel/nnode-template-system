from src.console.command import Command
from typing import AnyStr

class CommandFactory:
    @staticmethod
    def get_instance(name: AnyStr) -> Command:
        activated = list(filter(lambda comm: comm.NAME.upper() == name.upper(), Command.__subclasses__()))
        if len(activated) == 1:
            return activated[0]()
        raise Exception("Unknown command")
