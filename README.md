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
    },
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

### <u>Reserved types</u>

#### \<root element\>

The root element is always a typeless object. Named members of this object can be binded and is a way of changing values when imported in template. **NOTE**: if a root named member is binded, that member may only have basic types and can not be re-assigned with typed objects or other bindings.

#### meta

This can be stored in any object which can store none typed objects and other basic types. At the root of `meta` all members must be named and not repreated. Each instance will have a member version with a default of `0.0.0`. `meta` can only be stored as an unnamed object memeber once per object. `meta` data is provided to later stages to allow for proper parsing of the data.

#### template

This will not be present in the model and allows for structures to be predefined/imported. This can only be defined at the root.

    `name`:  Required of each template and must be unique. String name follows same rules as typed objects. When used, the name is used and is treated like any other typed object.

<u>template data</u>

    `import`: String path of external model. By default this is always relative path unless import routine is overriden.

**-OR-**

    `model`: Rather than import from an external source, the templated model can be defined internally. The structure must follow the same rules as the `root element`

#### bind

Binds a named value from the root element that it belongs to. Bound member must only have basic types contained (can not have typed objects or bindings). Below is an example:

```
{
    a: "hello world",
    Example
    {
        text: bind { name: a }
    },
    b: Example
    {
        SubExample
        {
            text: bind { name: "a" }
        }
    }
}
```

#### c

`c` is short for comment. Although technically parsed, it is not part of the model. To fully ensure no parsing errors occur, the object can enclose content in a multi-line string literal. Example:

```
{
    a: 1,
    c { "a comment" },
    b: "part of model", c{"another comment"},
    c {"""
none of {  this } will be parsed and will be ignored
{ without error
"""}
}
```

Although not part of the model, it is preserved such that rewritting a model to file will preserve them so long as it's parent remains and they haven't been explicitly removed.

#### prefill

Overrides/assigns default values for a given typed object. The the prefill value can either be a binding or a baisic type. `prefill`  must be specified at root. Example:

```
{
    prefill
    {
        type: Dog,
        values: { legs: 4, sound: "woof" }
    },
    Dog { }
}
```

**-OR-**

```
{
    a: { legs: 4, sound: "woof" },
    prefill
    {
        type: Dog
        values: bind { name: "a" }
    },
    Dog { }
}
```

**Becomes...**

```
{
    Dog
    {
        legs: 4,
        sound: "woof"
    }
}
```



### <u>Naming convention</u>

#### Types

* Starts with a letter

* Alpha-numeric

* Can have `_` or `-`

* *Nice to have:* Starts with upper case letter to avoid mixing with reserved types

#### Member name

* Nice to have:
  
  * Alpha-numeric
  
  * Starts with a lowercase letter

* Not a multi-line string litteral
