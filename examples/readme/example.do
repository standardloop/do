namespace main {
    vars {
        greeting="Hello!"
        name=" Standardloop"
    }
    task main {
        vars {
            example="local variable"
        }
        cmds {
            echo $greeting $name
            echo $example
        }
    }
}
