from abc import ABCMeta, abstractmethod
from proxy_backend import get_template_class, run_generator_save

class StateMachine(metaclass=ABCMeta):
    NAME = ""

    @abstractmethod
    def run(self):
        pass


class VIPSingleEqSM(StateMachine):
    NAME = "vip"

    def run(self):
        template = get_template_class(VIPSingleEqSM.NAME)
        print("Insert the interval of x:")
        print("x_min =", end=" ")
        x_min = float(input().split()[0])
        template.x_min = x_min
        print("x_max =", end=" ")
        x_max = float(input().split()[0])
        template.x_max = x_max
        print("Insert the number of x_i:", end=" ")
        nxi = int(input().split()[0])
        template.number_of_xi = nxi
        print("Insert the initial value:")
        print("X0 =", end=" ")
        x0 = float(input().split()[0])
        template.x0 = x0
        print("Y0 =", end=" ")
        y0 = float(input().split()[0])
        template.y0 = y0
        print("Insert the direction of save template:", end="\t")
        dir = input()
        print("Insert the file name:", end="\t")
        fn = input()
        final_dir = run_generator_save(dir, fn, template)
        print(f"Successfully!\nFile created on : \'{final_dir}\'")


class PVIPSingleEqSM(StateMachine):
    NAME = "pvip"

    def run(self):
        template = get_template_class(PVIPSingleEqSM.NAME)
        print("Insert the interval of x:")
        print("x_min =", end=" ")
        x_min = float(input().split()[0])
        template.x_min = x_min
        print("x_max =", end=" ")
        x_max = float(input().split()[0])
        template.x_max = x_max
        print("Insert the number of x_i:", end=" ")
        nxi = int(input().split()[0])
        template.number_of_xi = nxi
        print("Insert the interval of a:")
        print("a_min =", end=" ")
        a_min = float(input().split()[0])
        template.a_min = a_min
        print("a_max =", end=" ")
        a_max = float(input().split()[0])
        template.a_max = a_max
        print("Insert the number of a_i:", end=" ")
        nai = int(input().split()[0])
        template.number_of_ai = nai
        print("Insert the initial value:")
        print("X0 =", end=" ")
        x0 = float(input().split()[0])
        template.x0 = x0
        print("Y0 =", end=" ")
        y0 = float(input().split()[0])
        template.y0 = y0
        print("Insert the direction of save template:", end="\t")
        dir = input()
        print("Insert the file name:", end="\t")
        fn = input()
        final_dir = run_generator_save(dir, fn, template)
        print(f"Successfully!\nFile created on : \'{final_dir}\'")
