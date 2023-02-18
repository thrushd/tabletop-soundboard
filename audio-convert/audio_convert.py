import os
import subprocess
from pathlib import Path
from tkinter import Tk, Label, Button, Listbox, StringVar, filedialog


class Converter:
    def __init__(self, driver):
        self.driver = driver

        driver.title("Teensy Audio Converter")
        driver.resizable(height=False, width=False)

        self.input_path = Path()
        self.output_path = Path()
        self.input_path_text = StringVar()
        self.output_path_text = StringVar()

        self.input_path_text.trace('w', self.validate)
        self.output_path_text.trace('w', self.validate)

        self.files = []

        # input row
        self.lbl_input_text = Label(master=driver, text="Input Directory:")
        self.lbl_input_text.grid(row=0, column=0, sticky='E')

        self.lbl_input_path = Label(
            master=driver, textvariable=self.input_path_text)
        self.lbl_input_path.grid(row=0, column=1)

        self.btn_input_browse = Button(
            text="Browse Input", command=self.browse_input)
        self.btn_input_browse.grid(row=0, column=2, sticky='W')

        # output row
        self.lbl_output_text = Label(master=driver, text="Output Directory:")
        self.lbl_output_text.grid(row=1, column=0, sticky='E')

        self.lbl_output_path = Label(
            master=driver, textvariable=self.output_path_text)
        self.lbl_output_path.grid(row=1, column=1)

        self.btn_output_browse = Button(
            text="Browse Output", command=self.browse_output)
        self.btn_output_browse.grid(row=1, column=2, sticky='W')

        self.lst_files = Listbox(master=driver, width=50)
        self.lst_files.grid(row=2, column=0, columnspan=3)
        self.lst_files.bindtags((self.lst_files, driver, 'all'))

        self.btn_convert = Button(text="Convert", command=self.convert)
        self.btn_convert.grid(row=3, column=1)
        self.btn_convert.config(state='disabled')

    def browse_input(self):
        self.input_path = Path(filedialog.askdirectory())
        print(type(self.input_path))
        self.input_path_text.set(str(self.input_path))
        os.chdir(self.input_path)
        self.files = [f for f in os.listdir(self.input_path) if os.path.isfile(f)]

        for file in self.files:
            self.lst_files.insert('end', file)

    def browse_output(self):
        self.output_path = Path(filedialog.askdirectory())
        self.output_path_text.set(str(self.output_path))

    def validate(self, *args):
        x = self.input_path_text.get()
        y = self.output_path_text.get()

        if x and y:
            self.btn_convert.config(state='normal')
        else:
            self.btn_convert.config(state='disabled')

    def convert(self):
        print("converting!")

        for input in self.files:
            output = input.split('.')[:-1]
            output = '.'.join(output)
            output = output + '.wav'
            output = self.output_path / output
            subprocess.call(['ffmpeg', '-n', '-i', input, '-ar', '44100', str(output)])


if __name__ == "__main__":
    root = Tk()
    my_converter = Converter(root)
    root.mainloop()
