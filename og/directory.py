# user directory

import uasyncio as asyncio

keymap = (
    "0",  # 0
    "4",  # 1
    "8",  # 2
    "A",  # 3, ENT
    "1",  # 4
    "5",  # 5
    "9",  # 6
    "Z",  # 7, DEL
    "2",  # 8
    "6",  # 9
    "*",  # 10, SEQ
    "CALL",  # 11, TKO
    "3",  # 12
    "7",  # 13
    "#",  # 14, RST
    "?",  # 15, ABT
)

MAX_CODE_LEN = 8

DIR_STATE_NONE = 0
DIR_STATE_USER = 1
DIR_STATE_CODE = 2


class lookup:
    def __init__(self, key_queue, out_queue, codes, users):
        self.key_queue = key_queue
        self.out_queue = out_queue
        self.code = codes
        self.users = users
        self.state = DIR_STATE_NONE
        self.code = []
        self.user_idx = 0

    async def run(self):
        while True:
            down, val = await self.key_queue.get()
            if not down:
                continue
            key = keymap[val]

            if self.state == DIR_STATE_CODE:
                if key in "0123456789":
                    if len(self.code) < MAX_CODE_LEN:
                        self.code.append(key)
                        print("check %s" % "".join(self.code))
                    else:
                        self.state = DIR_STATE_NONE
                elif key == "*":
                    self.state = DIR_STATE_CODE
                    self.code = []
                elif key == "A":
                    self.state = DIR_STATE_USER
                    self.user_idx = 0
                elif key == "Z":
                    self.state = DIR_STATE_USER
                    self.user_idx = len(self.users) - 1
                else:
                    self.state = DIR_STATE_NONE

            elif self.state == DIR_STATE_USER:
                if key == "A":
                    if self.user_idx > 0:
                        self.user_idx -= 1
                        print("idx %d" % self.user_idx)
                elif key == "Z":
                    if self.user_idx < len(self.users) - 1:
                        self.user_idx += 1
                        print("idx %d" % self.user_idx)
                elif key == "CALL":
                    name, number = self.users[self.user_idx]
                    print("dial %s @ %s" % (name, number))
                    self.state = DIR_STATE_NONE
                else:
                    self.state = DIR_STATE_NONE

            else:
                if key == "*":
                    self.state = DIR_STATE_CODE
                    self.code = []
                elif key == "A":
                    self.state = DIR_STATE_USER
                    self.user_idx = 0
                elif key == "Z":
                    self.state = DIR_STATE_USER
                    self.user_idx = len(self.users) - 1
                else:
                    self.state = DIR_STATE_NONE
