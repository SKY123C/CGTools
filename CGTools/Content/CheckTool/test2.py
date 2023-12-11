class A:
    x = 1
    

class B(A):
    a = 1
    def __dict__(self):
        ...
object
c = B()
print(B.__dict__)
print(A.__bases__)
print(type(A))
type