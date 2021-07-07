from abc import ABCMeta, abstractmethod
from sys import exit
from colorama import Fore, Style
from constant import MAIN_CYCLE, LINUX
from proxy_backend import get_all_meta_templates
from state_machines import StateMachine

class Command(metaclass=ABCMeta):
    NAME = ""
    DESCRIPTION = ""

    @abstractmethod
    def execute(self):
        pass

class ExitCommand(Command):
    NAME = "exit"
    DESCRIPTION = "Close the app"

    def execute(self):
        print((Fore.RED if LINUX else "") + "Are you sure? [y/n]" + (Fore.RESET if LINUX else ""), end="\t")
        line = input()
        if line.upper() == "Y":
            exit()

class CommandsCommand(Command):
    NAME = "commands"
    DESCRIPTION = "Show all commands"

    def execute(self):
        comm = MAIN_CYCLE["COMMAND"]
        for command in [(comm.NAME, comm.DESCRIPTION) for comm in Command.__subclasses__()]:
            comm += f"\n\t{Fore.YELLOW if LINUX else str()}{command[0]} {Fore.RESET if LINUX else str()}- {command[1]}"
        return comm

class TemplateCommand(Command):
    NAME = "templates"
    DESCRIPTION = "Show a menu with all templates"

    def execute(self):
        menu = MAIN_CYCLE["MENU"]
        for template in get_all_meta_templates():
            menu += f"\n\t{Fore.YELLOW if LINUX else str()}{template[0]} {Fore.RESET if LINUX else str()}- {template[1]}"
        print(menu)
        print((Style.BRIGHT if LINUX else str()) + MAIN_CYCLE["PROMPT"], end="\t")
        selected = input()
        for sm in StateMachine.__subclasses__():
            if sm.NAME == selected:
                sm().run()
                return
        return "Unknown template"
