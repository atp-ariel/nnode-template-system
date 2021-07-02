from typing import Dict, List, AnyStr
from src.console.command_factory import CommandFactory
from colorama import Fore, Style
from src.console.constant import MAIN_CYCLE, LINUX

def print_yellow(txt: str, bold=True):
    print(((Fore.YELLOW + (Style.BRIGHT if bold else "")) if LINUX else "") + txt + (Fore.RESET if LINUX else ""))

def print_prompt():
    print((Style.BRIGHT if LINUX else str()) + MAIN_CYCLE["PROMPT"], end="\t")

def print_welcome():
    print_yellow(MAIN_CYCLE["WELCOME"])
    get_commands_options()

def get_commands_options() -> List:
    print_yellow(CommandFactory.get_instance("commands").execute(), True)

def command_detection(line: AnyStr) -> None:
    ret = CommandFactory.get_instance(line).execute()
    if isinstance(ret, str):
        print_yellow(ret)

def verify_kill():
    print((Fore.RED if LINUX else "" )+ "Are you sure? [y/n]")
    line = input()
    if line.upper() == "Y":
        CommandFactory.get_instance("exit").execute()
