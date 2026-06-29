# _Sample project_

This is the simplest buildable example. The example is used by command `idf.py create-project`
that copies the project to user specified path and set it's name.

## Example folder contents

The project **sample_project** contains one source file in C language [app.c](app/app.c). The file is located in folder [app](app).

The project build configuration is contained in `CMakeLists.txt` files that provide set of directives and instructions 
describing the project's source files and targets (executable, library, or both). 

Below is short explanation of reapping files in the project folder.

```
├── CMakeLists.txt
├── app
│   ├── CMakeLists.txt
│   └── app.c
└── README.md                  This is the file you are currently reading
```
