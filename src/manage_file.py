from pathlib import Path
from typing import AnyStr
from constant import CPP_EXTENSION

class ManagerFile:
    @staticmethod
    def save_template(directory: AnyStr, name_file: AnyStr, content: AnyStr):
        directory = Path(directory)
        if not directory.is_dir():
            raise Exception(f"No such folder {directory}")
        if not name_file[-4:] == CPP_EXTENSION:
            name_file += CPP_EXTENSION
        name_file = ManagerFile.__set_correct_name(name_file, directory)
        template = open(directory / name_file, "w+")
        template.write(content)
        template.close()
        return directory / name_file

    @staticmethod
    def __set_correct_name(name_file: AnyStr, directory: Path):
        files = [x for x in directory.glob('*.*')]
        if directory / name_file not in files:
            return name_file
        name_file = "pvip " + name_file
        return ManagerFile.__set_correct_name(name_file, directory)