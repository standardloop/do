namespace main {
    vars {
        x="global var"
    }
    task overwritten {
        vars {
            x="task var overwritten"
        }
        cmds {
            echo "$x"
        }
    }
    task take-from-global {
        cmds {
            echo "$x"
        }
    }

    task no-vars {
        cmds {
            echo "${missing}"
        }
    }

    task call-no-vars-with-local {
        vars {
            missing="no missing!"
        }
        cmds {
            no-vars
        }
    }

    task with-brace {
        cmds {
            echo "${x}" | tr '[:lower:]' '[:upper:]'
        }
    }
}
