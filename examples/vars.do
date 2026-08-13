namespace main {
    vars {
        x="hello"
    }
    task take-from-global {
        cmds {
            echo "$x"
        }
    }
    task will-it-ovewrite {
        cmds {
            echo $x
        }
    }
    task overwritten {
        cmds {
            x="overwritten"
            will-it-ovewrite
        }
    }
}
