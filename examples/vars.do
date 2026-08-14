namespace main {
    vars {
        x="hello"
    }
    task overwritten {
        vars {
            x="overwritten"
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
}
