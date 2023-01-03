class State:
    def __init__(self):
        self.value = None

    def is_valid(self):
        if self.value is None:
            return False
        return True

    def set(self, val):
        if val is None:
            return False
        self.value = val
        return True

    def get(self):
        return self.value

class Node:
    def __init__(self, cb):
        self.cb = cb

    def new_cb(self, cb):
        self.cb = cb

    def update(self, val):
        self.cb(val)

if __name__ == '__main__':
    state = State()
    new_state = State()
    # lamb = lambda new_val, state=state: state.set(new_val)
    # 
    # if state.get() is None:
    #     print('first check is None')

    # if not lamb(None):
    #     print('cant set new val as None')

    # if lamb(123):
    #     print('set to 123')
    # print(state.get())

    n = Node(state.set)
    n.update(123)

    print(state.get())

    n.new_cb(new_state.set)
    n.update(1235)
    print(state.get())
    print(new_state.get())
