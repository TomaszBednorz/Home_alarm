import tkinter
import tkinter as tk
import can
import time
from PIL import Image, ImageTk

import serial.tools.list_ports
import serial


class GUI:
    def __init__(self):
        self.COM_PORT = 'COMX'
        self.IR_data = 0
        self.US_data = 0

        self.root = tk.Tk()
        self.root.title('Sensor GUI')
        self.root.geometry("600x400")

        self.startup_page()
        self.root.mainloop()


    def receive_can_msg(self):
        # untested
        with can.Bus(interface='serial', channel=self.COM_PORT, bitrate=115200) as bus:
            printer = can.Printer()
            logger = can.Logger('dane.txt')
            buffer_reader = can.BufferedReader()
            # msg = bus.recv()
            ctr = 0
            # printer.on_message_received(msg)
            # logger.on_message_received(msg)
            self.msg = buffer_reader.get_message()

            # printer.stop()
            # logger.stop()
            # buffer_reader.stop()
            while True:
                self.root.update()
                if ctr % 3 == 0:
                    self.IR_data = self.msg
                    self.IR_data_string.set(str(self.IR_data))

                    if not isinstance(self.IR_data, int) or not self.IR_data:
                        try: self.IR_green.destroy()
                        except: pass
                        self.IR_red = tk.Label(self.data_frame, image=self.red_circle).grid(row=2, column=0, ipadx=20, ipady=20)

                    else:
                        try: self.IR_red.destroy()
                        except: pass
                        self.IR_green = tk.Label(self.data_frame, image=self.green_circle).grid(row=2, column=0, ipadx=20, ipady=20)


                else:
                    self.US_data = self.msg
                    self.US_data_string.set(str(self.US_data))
                    if not isinstance(self.US_data, int) or self.US_data < 50:
                        try: self.US_green.destroy()
                        except: pass
                        self.US_red = tk.Label(self.data_frame, image=self.red_circle).grid(row=2, column=1, ipadx=20, ipady=20)
                    else:
                        try: self.US_red.destroy()
                        except: pass
                        self.US_green = tk.Label(self.data_frame, image=self.green_circle).grid(row=2, column=1, ipadx=20, ipady=20)
                ctr += 1

                try:
                    self.msg = buffer_reader.get_message()
                except:
                    bus.shutdown()
    def receive_uart_msg(self):

        with serial.Serial(self.COM_PORT, 115200, timeout=1) as ser:
            while True:
                self.root.update()
                self.msg = str(ser.readline()).split(' ')
                if self.msg[0] == "b'16":
                    # self.IR_data = int(self.msg[1][:1])
                    self.IR_data = 1
                    self.IR_data_string.set(str(self.IR_data))
                    self.IR_red = tk.Label(self.data_frame, image=self.red_circle)
                    self.IR_red.grid(row=2, column=1, ipadx=20, ipady=20)
                    if not isinstance(self.IR_data, int) or not self.IR_data:
                        self.IR_red.config(image=self.red_circle)

                    else:
                        self.IR_red.config(image=self.green_circle)

                elif self.msg[0] == "b'5":
                    self.US_data = int(self.msg[1][:-5])
                    self.US_data_string.set(str(self.US_data))
                    self.US_red = tk.Label(self.data_frame, image=self.red_circle)
                    self.US_red.grid(row=2, column=0, ipadx=20, ipady=20)
                    if not isinstance(self.US_data, int) or self.US_data < 100:
                        self.US_red.config(image=self.red_circle)
                    else:
                        self.US_red.config(image=self.green_circle)

    def startup_page(self):

        self.startup_frame = tk.Frame(self.root)
        self.startup_frame.pack()

        label = tk.Label(self.startup_frame, text="Select COM port first", font=30, fg="blue")
        label.pack()

        available_ports = [comport.device for comport in serial.tools.list_ports.comports()]
        buttons = []

        for port in range(len(available_ports)):
            buttons.append(tk.Button(self.startup_frame, text=available_ports[port], width=8))
            buttons[-1].pack()
            buttons[-1].config(command=lambda i=available_ports[port]: self.select_serial_port(i))
            # buttons[-1].pack()

    def data_screen(self):
        self.data_frame = tk.Frame(self.root)

        label_sensor1 = tk.Label(self.data_frame, text="Czujnik ultradźwiękowy", font=30, fg="blue").grid(row=0, column=0, ipadx=20, ipady=20)
        label_sensor2 = tk.Label(self.data_frame, text="Czujnik IR", font=30, fg="blue").grid(row=0, column=1, ipadx=20, ipady=20)

        self.IR_data_string = tk.StringVar()
        self.US_data_string = tk.StringVar()

        self.data_label1 = tk.Label(self.data_frame, textvariable=self.US_data_string, font=30, fg="blue").grid(row=1, column=0, ipadx=20, ipady=20)
        self.data_label2 = tk.Label(self.data_frame, textvariable=self.IR_data_string, font=30, fg="blue").grid(row=1, column=1, ipadx=20, ipady=20)

        red_img = Image.open('red_circle.png').resize((100, 100))
        green_img = Image.open('green_circle.png').resize((100, 100))
        self.red_circle = ImageTk.PhotoImage(red_img)
        self.green_circle = ImageTk.PhotoImage(green_img)

        self.data_frame.pack()
        self.receive_uart_msg()

    def select_serial_port(self, port):
        self.COM_PORT = port
        print(self.COM_PORT)
        self.startup_frame.destroy()
        self.data_screen()

def main():
    gui = GUI()

if __name__ == "__main__":
    main()