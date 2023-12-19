# NexTOP - A linux System Monitoring TUI using C

<p>
NexTop is a terminal based application to track and monitor your linux system parameteres such as cpu usage, memory usage, running process details, etc, made using vanilla c and ncurses libarary. It is fast responsive and efficient.
</p>

<br/>

![Alt text](https://github.com/ADITYA-RAY/NexTOP/blob/main/screenshot.png?raw=true "Optional Title")

<br/>

<h2>How to Install (For Linux Systems)</h2>

- Download and extract the code.
- open terminal and cd to root directory of code.
- Run `gcc main.c -lncurses`
- Run `./a.out`

<h2>Overview</h2>
<p>
The project uses the proc filesystem is a pseudo-filesystem which provides an interface to kernel data structures.  It is commonly mounted at /proc.
</p>

<h2>Features</h2>
<p>
  <ul style="list-style-position: inside;">
      <li>Core Wise CPU usage monitoring</li>
      <li>Memmory usage monitoring</li>
      <li>Swap usage monitoring</li>
      <li>Running Process details tabulation</li>
  </ul>
</p>

<h3> Concepts Used</h3>

- Proc filesystem
- C file handling
- Dynamic memory allocation
- Terminal applications using ncurses

<h3> Challenges</h3>

- Proc filesystem data is unorgonized which can't be decoded easily and requires strict understanding of proc documentation.
- Presentation of data on terminal window.

<br/>
<h2>License</h2>
WebSerial is licensed under General Public License v3 ( GPLv3 ).
<br/>
<br/>
<img src="https://img.shields.io/github/license/ayushsharma82/WebSerial.svg?style=for-the-badge" />
</div>
