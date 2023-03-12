# display scroller

class scroll:

    def __init__(self, msg, dirn, width, puts):
        assert len(msg) > 0, "bad msg"
        assert type(dirn) == bool, "bad dirn"
        assert width > 0, "bad width"
        self.msg = msg
        self.dirn = dirn
        self.width = width
        self.puts = puts
        self.n = 0
        self.output()

    def output(self):
        s = [self.msg[k % len(self.msg)] for k in range(self.n, self.n + self.width)]
        self.puts(''.join(s))

    def update(self):
        self.n += (1,-1)[self.dirn]
        self.n %= len(self.msg)
        self.output()




