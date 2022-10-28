# PerforceClientConfiguration
A tool to help automatically configure new Perforce clients by running when a new workspace has been setup


# Disclaimers
The code assumes that your workspace name and workspace root directory folder are the same.
It is generally good practice to do this, but if you do not there is an infinite loop

It also assumes there is a .p4ignore file, which is how it determines where the root folder is. The tool sits inside of a nested Tools/.. directory in our project.

Compiling the current codebnase requires Qt: https://www.qt.io/


# Info
The code can be adopted to your own needs, we update it with the settings that we use internally.
If you have suggestions for improvement, be it code or settings that we do not use, please do reach out! ❤️
