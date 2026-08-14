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

    task with-brace {
        cmds {
            echo "${x}"
        }
    }
}
