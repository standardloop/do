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
