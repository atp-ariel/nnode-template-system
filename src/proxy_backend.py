from template import Template
from typing import AnyStr, Tuple
from manage_file import ManagerFile

def get_template_class(template_name: AnyStr):
    for template in Template.__subclasses__():
        if template.NAME == template_name:
            return template()
    return None

def get_all_meta_templates() -> Tuple:
    for template in Template.__subclasses__():
        yield template.NAME, template.DESCRIPTION


def run_generator_save(dir: AnyStr, filename: AnyStr, template: Template):
    content = template.generate()
    return ManagerFile.save_template(dir, filename, content)