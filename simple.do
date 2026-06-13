namespace main {
    task main {
        cmds {
            x="hello"
            echo $x
        }
    }
    task test {
        cmds {
            x="test"
            echo $x
        }
    }
}
