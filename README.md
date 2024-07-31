<h1 style="color: #006cff; font-size: 50px;"><img style="margin-bottom: -15px;" src="doc/res/logoSolid.png" width="70"/> SOFT DEVELOPMENT </h1>

# libJAL

<img style="margin-bottom: -15px;" src="doc/res/wip.png" width="660"/>

libJAL is still being created; libstrext a string extension library has been created with an initial release. See release folder for libraries.

## About

JAL is short for JSON Adjacent Layout. JAL on first glance appears similar JSON, but with additional functionality and fewer contraints. libJal can also parse JSON (in most cases) as JAL behaves as a superset of JSON.

### <u>The differences</u>

#### Everything as string and staged interpretation

When parsing JAL, everything is first parsed out as a string including both the member and the value. So `"x":1` in <u>JSON</u>, **1** is an integer and **x** is a string, whereas in <u>JAL</u> both start as strings and is left to later stages to interpret values as needed. This allows for things such as enumerations to be defined and evaluated in later stages. 

#### Quotes optional

Where each member in JSON must be enclosed in quotes, JAL only requires it if spaces are present (although spaces are discouraged as a naming convention). This also means values don't need it either if a space isn't present such as `name: bob`. **NOTE:** single character quotes (`'`) are treated like regular characters and can't enclose a string.

#### No Backslash escape character parsing

No backslash escape characters are parsed out to allow the string to be handled as needed. This avoids the annoyance of needing to double escape a character when needing to preserve the escape character. The only exception are quotes which can be escaped with tilde (`"... ~" ..."`>>` ... " ...`) and double tilde if tilde is desired (`"... ~~"`>>`... ~`). A single tilde not followed by a quote will be treated as a regular tilde but pairs will resolve to single tilde.

### Multi-line string literal

Tripple quotes are used as a multi-line string with a backslash infront eliminating all white space up to the first new line and a trailing backslash to do the same for the back. So `\"""<your multi-line string literal>"""\`will result in all white space up to the first new line (or non-whitespace) to be trimmed. The trimming is convient if you wish to preserve text alignment. Removing the backslashes will avoid the trimming if undesired.

#### Typed members

As part of the staged interpretation, objects can be be typed to allow for value refinement and format enforcement. Below, Rectangle is a typed object with members: x,y,w, and h.

**Example:**

```
{
    shapes: Rectangle
    {
        x: 0,
        y: 0,
        w: 10,
        h: 100
    }
}
```

#### Named and unnamed members

Each object can have named and unnamed members, where unnamed are objects aren't preceded with `<name>: `

**Example:**

```
{
    Rectangle
    {
        x: 0,
        y: 0,
        w: 10,
        h: 100
    }
    Rectangle
    {
        x: 10,
        y: 10,
        w: 10,
        h: 100
    }
}
```

#### Order preserved and appended

The order of members are preserved such that if given objects Cat, Dog, Horse and you specify objects in the order of Cat, Dog, Cat, Horse; they will be ordered that way when parsed. Furthermore if a member name is used multiple times, it's equivalent to an array such that `a:[1,2,3], a:4, a:[5,6], a:0` is equal to `a[1,2,3,4,5,6,0]`

#### Commas optional for object members

Objects don't require commas between member. **However** arrays/lists require them.

### <u>Reserved types</u>

TBD...
