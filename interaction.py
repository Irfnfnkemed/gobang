import socket
import threading
import time

from matplotlib import pyplot as plt


class Interaction:
    def __init__(self):
        self.win = False
        self.last_piece = None
        self.end = False
        self.fig, self.ax = plt.subplots(facecolor='slategray')
        plt.rcParams['font.sans-serif'] = ['SimSun', 'Arial']
        self.ax.set_xlim(0, 16)
        self.ax.set_ylim(0, 16)
        plt.gca().set_aspect('equal')
        plt.gca().set_xticks([])
        plt.gca().set_yticks([])
        for i in range(2, 15):
            plt.plot([i, i], [1, 15], color='black', linewidth=0.5)
        for j in range(2, 15):
            plt.plot([1, 15], [j, j], color='black', linewidth=0.5)
        plt.plot([1, 1], [1, 15], color='black', linewidth=0.8)
        plt.plot([15, 15], [1, 15], color='black', linewidth=0.8)
        plt.plot([1, 15], [1, 1], color='black', linewidth=0.8)
        plt.plot([1, 15], [15, 15], color='black', linewidth=0.8)
        self.pos_x = 0
        self.pos_y = 0
        self.is_player = True
        self.occupy_set = set()
        self.is_player_lock = threading.Lock()

    def set(self):
        self.fig.canvas.mpl_connect('button_press_event', self.on_click)
        plt.show()

    def on_click(self, event):
        if event.xdata is not None and event.ydata is not None and self.is_player and not self.end:
            self.pos_x = self.get_close_int(event.xdata)
            self.pos_y = self.get_close_int(event.ydata)
            if ((self.pos_x << 4) | self.pos_y) not in self.occupy_set:
                self.draw_piece()
                thread = threading.Thread(target=self.ai_handle, args=(str((self.pos_x << 4) | self.pos_y),))
                thread.start()

    @staticmethod
    def get_close_int(pos):
        if pos < 0.0:
            return 0
        elif pos > 15.0:
            return 15
        elif pos - int(pos) < 0.5:
            return int(pos)
        else:
            return int(pos) + 1

    def draw_piece(self):
        if self.last_piece is not None:
            self.last_piece.set_edgecolor('white')
        self.occupy_set.add((self.pos_x << 4) | self.pos_y)
        self.last_piece = plt.Circle((self.pos_x, self.pos_y), radius=0.45, edgecolor='red', facecolor='black' if self.is_player else 'gray')
        self.ax.add_patch(self.last_piece)
        self.is_player_lock.acquire()
        try:
            self.is_player = not self.is_player
        finally:
            self.is_player_lock.release()
        time.sleep(0.1)
        plt.draw()

    def ai_handle(self, data):
        cpp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_address = ('localhost', 6666)  # C++后端服务器地址和端口
        cpp.connect(server_address)
        cpp.sendall(data.encode())
        response = int(cpp.recv(32).decode())
        cpp.close()
        if response > 0b11111111:
            if bool(response >> 18):
                self.ax.text(8.5, 17, "你赢了！", fontsize=20, color='black', ha='center', va='center')
            else:
                self.ax.text(8.5, 17, "你输了！", fontsize=20, color='black', ha='center', va='center')
                self.pos_x = (response >> 4) & 0b1111
                self.pos_y = response & 0b1111
                self.draw_piece()
            beg_pos = (response >> 10) & 0b11111111
            interval = [1, 16, 17, 15][(response >> 8) & 0b11]
            for i in range(5):
                self.ax.scatter((beg_pos + interval * i) >> 4, (beg_pos + interval * i) & 0b1111, color='red', marker='x', s=30)
            time.sleep(0.1)
            plt.draw()
            self.end = True
        else:
            self.pos_x = (response >> 4) & 0b1111
            self.pos_y = response & 0b1111
            self.draw_piece()


a = Interaction()
a.set()
