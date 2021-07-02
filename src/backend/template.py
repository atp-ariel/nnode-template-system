from abc import ABCMeta, abstractmethod
from typing import Tuple, AnyStr
from json import load

class Template(metaclass=ABCMeta):
    NAME = ""
    DESCRIPTION = ""

    # region Properties
    @property
    def template_file(self) -> AnyStr:
        return self.__template_file
    # endregion Properties

    # region Constructor
    def __init__(self, template_file: AnyStr):
        self._config = {}
        self.__template_file = template_file

    # endregion Constructor

    # region Methods
    @abstractmethod
    def generate(self) -> AnyStr:
        pass

    def _insert_configuration(self, kv_pair: Tuple):
        if not len(kv_pair) == 2:
            raise Exception()
        if isinstance(kv_pair[0], str):
            raise Exception()

        self._config[kv_pair[0]] = kv_pair[1]
    # endregion Methods


class VIPSingleEquation(Template):
    NAME = "single_vip"
    DESCRIPTION = "Solve VIP of a single ODE"

    @property
    def x_min(self) -> AnyStr:
        return self._config["x_min"]

    @x_min.setter
    def x_min(self, x: float):
        self._config["x_min"] = x

    @property
    def x_max(self) -> AnyStr:
        return self._config["x_max"]

    @x_max.setter
    def x_max(self, x: float):
        self._config["x_max"] = x

    @property
    def number_of_xi(self):
        return self._config["number_of_xi"]

    @number_of_xi.setter
    def number_of_xi(self, x: int):
        self._config["number_of_xi"] = x

    @property
    def x0(self):
        return self._config["x0"]

    @x0.setter
    def x0(self, x: float):
        self._config["x0"] = x

    @property
    def y0(self):
        return self._config["y0"]

    @y0.setter
    def y0(self, x: float):
        self._config["y0"] = x

    @property
    def function(self):
        return self._config["function"]

    @function.setter
    def function(self, func: AnyStr):
        self._config["function"] = func

    def __init__(self):
        super().__init__("./backend/__templates__/simple-without-params.json")

    def generate(self) -> AnyStr:
        json_template = load(open(self.template_file, "r"))
        priority = sorted(json_template, key=lambda x: json_template[x]["priority"])

        generated = ""
        for node_name in priority:
            if node_name != "function" and node_name in self._config.keys():
                generated += self.__generate_var(node_name, json_template[node_name]["content"])
            elif node_name == "function":
                generated += self.__generate_func(json_template[node_name]["content"])
            else:
                generated += json_template[node_name]["content"]
        return generated

    def __generate_var(self, name: AnyStr, content: AnyStr) -> AnyStr:
        var = content
        var += str(self._config[name])
        var += ";\n"
        return var

    def __generate_func(self, content: AnyStr):
        var = content
        var += "{\n\t"
        if "function" not in self._config.keys():
            var += "// ! Put your function here"
        else:
            var += self._config["function"]
        var += "\n}\n\n"
        return var
