from typing import Optional, Union
import unreal
from unreal import Name, Object


''''
收集器类型
收集器属性：
    1. 路径(Path)
    2. Families
    3. Label
    4. active
    5. version
    6. order
收集对象
'''

'''
检查项类型
检查项类型
order
'''
'''
class UCollector
class UVa
'''

@unreal.uclass()
class MyFirstCheck(unreal.CheckBasic):
	
    def _post_init(self):
        self.set_display_field_name("MyFirstCheck1")
    
    @unreal.ufunction(override=True)
    def operation(self) -> bool:
        print("test1")
        return True

@unreal.uclass()
class Collect(unreal.CheckCollector):
    
    @unreal.ufunction(override=True)
    def operation(self, context) -> bool:
        print("test1")
        context.create_instance()
        return True
    

    
test = MyFirstCheck()


