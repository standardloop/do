# do

https://github.com/standardloop/do


[![GitHub Release](https://img.shields.io/github/v/release/standardloop/do?sort=semver)](https://github.com/standardloop/do/releases) ![Platform: macOS](https://img.shields.io/badge/platform-macOS-000000?style=flat&logo=apple&logoColor=white) ![C Version](https://img.shields.io/badge/C_Standard-C17-00599C?logo=c&logoColor=white)

## What is Do?

`do` is a command runner like GNU make, go-task, or just.

`do` is written in C.

When running a task from a `dofile`, the file is lexed and then parsed.

Then it will see if the `task` has a `check`. If the `check` fails, the `cmds` will be run.

`vars` can be `namespace` scoped or `task` scoped.

`task` `cmds` and `check` can call other `tasks`. 

All `cmds` are ran in one shell, as the runner currently envokes a `sh -c`.

## Example File

```
namespace main {
    vars {
        greeting="Hello!"
        name=" Standardloop"
    }
    task main {
        vars {
            example="task scoped variable"
        }
        cmds {
            echo $greeting $name
            echo $example
        }
    }
}
```

## Running

```sh
$ ./do-task -f ./examples/readme.do
Hello! Standardloop
task scoped variable
```


## Structure

A dofile has the following sections:

- `namespace`
- `vars` (namespace scoped and task scoped)
- `cmds`
- `check`

### Namespace

A `namespace` is what groups all tasks and variables together

### Variables

Variables are just bash variables and can be scoped to a namespace or a specific task.

Task specific variables will overwrite global ones, example:

```
namespace main {
    vars {
        name="foo"
    }
    task main {
        vars {
            name="bar"
        }
        cmds {
            echo $name
        }
    }
}
```

```sh
$ ./do-task -f ./examples/readme/task-var-overwrite-global.do  
bar
```

### Commands

`cmd`s are what will be run. Think of this as just a sectioned off bash script.

`cmds` can call other tasks, example:
```
namespace main {
    task call-me {
        cmds {
            echo "call-me task"
            echo $example
        }
    }
    task main {
        vars {
            example="defined in main, but called tasks can see"
        }
        cmds {
            call-me
        }
    }
}
```
```sh
$ ./do-task -f ./examples/readme/cmds-call-other-task.do
call-me task
defined in main, but called tasks can see
```

### Check

A `check` is what will be checked before a `task's` `cmds` will be run.

If a `check` fails (doesn't exit 0), the `cmds` **will** be run.

If a `check` succeeds (exits 0), the `cmds` will not be run, because the `task` is considered up to date.

Example:

```
namespace main {
    task will-run {
        check {
            exit 1
        }
        cmds {
            echo "this will run, because check will fail"
        }
    }

    task will-not-run {
        check {
            exit 0
        }
        cmds {
            echo "this will not run, because check succeeds"
        }
    }
}
```
```sh
$ ./do-task -f ./examples/readme/check.do -t will-run   
this will run, because check will fail
$ ./do-task -f ./examples/readme/check.do -t will-not-run
[INFO] task will-not-run is up to date, nothing to run!
```

If no `check` is defined, then the cmds will always run.


## Building

TODO

## Downloading

TODO
