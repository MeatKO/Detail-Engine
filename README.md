# Detail Engine

![dtl_logo](https://user-images.githubusercontent.com/22328021/74090309-5b066000-4ab2-11ea-81ac-321f0406f31a.png)

# Entity Component System
The Entity Component System (ECS) is a design pattern that prioritizes the `Composition Over Inheritance`
principle that is used to make flexible games that define every game object as an `Entity`.
This allows the game creator to freely add and remove specific traits to each `Entity`.
The traits that an `Entity` can have are called `Components` and they only hold Data.
Using ECS leads to the segregation of the Data and the things doing work on it, the workers in this case being the `Systems`.
This helps remove Inheritance problems and prevents the usage of code unrelated to specific `Systems`.  
Example :  
Calling `Achievement-System` code from the Physics Engine when a player goes to a specific location.  
This would add hidden functionality in the Physics Engine that would later need to be changed everywhere if a small change is done.

### Entities
In many ECS implementations the `Entities` are just IDs stored in the ECS controller/manager.
In that case, the Entity doesnt have direct knowledge of the `Components` it owns and the `Components` have to contain their Entity ID.
The controller/manager has to store the `Components` somewhere, most likely in a list and if there are many `Entities` and `Components`,
the `Systems` have to potentially loop trough a big number of elements for each `Entity` to find out their IDs. 
This is not necessarily bad and better solutions may exist, but I am not yet aware of them as everyone has their own approach when it
comes to ECS.

### Components
`Components` are just Data holders attached to the `Entities` one way or another. The Detail Engine stores the `Components` in vectors
contained in the `Entities` themself. When it comes to the Detail Engine, the `Components` also store any type of Data and hold a
`ComponentType` variable which can be used to find the type of Data used in them as the Data Type is uniform for all `Components` of
the same Type

### Systems
`System` in the Detail Engine is considered everything that has access to the `Entity Controller` and does some type of work on its
`Entities` and `Components`. The `Entity Controller` functionality has thread synchronisation and the `Systems` can be on different 
threads.

![dtl_pcs_ecs](https://user-images.githubusercontent.com/22328021/74090871-51342b00-4ab9-11ea-86e0-ccae2be81c9a.png)

# Publisher Channel Subscriber
As shown in the picture above, the Detail Engine architecture doesn't just make use of ECS, as it is just a way of managing Entities and
Components.
We need a way of communication between the different `Systems` and not only - `User Input` is not a `System` yet it has influence over 
what is being done in the program, the `Networking` isn't necessarily a `System` but also potentially has to know what the user
has done, so this leads to the introduction of `Messages` - things sent between different parts of our program and
`Publishers`, `Channels` and `Subscribers`.

### Messages
So before we talk about ways of communication which is basically just transportation of Data between our Engine's `Systems` lets
first see what is being sent.  
A `Message` is just a container/holder of some sort of Data ( Actually any type of data you give it ) which is very similar to 
`Components` as they also store any type of data and have a Type, but unlike `Components` a `Message` is only good once - 
it gets sent from a `Publisher` to a `Channel`, distributed to its `Subscribers` after that it has no purpose and gets destroyed.

### Publishers
`Publishers` are an inheritable class that allow their Derived class to Send `Messages` to a `Message Bus` which in this case
are called `Channels`.
The `Publisher` serves for only one thing as it doesn't Receive messages and has no functionality to perform actions based on 
`Message` receiving. I must point out that a class can Inherit both `Publisher` and `Subscriber` and be able to both Send `Messages`
to the `Bus` and Receive them. The `Publisher` class holds a pointer to `Channel` objects and directly accesses their members 
in order to push `Messages` to them.

### Channels
The `Channel` is an inheritable class that has the ability to both Send and Receive `Messages`, it holds pointers to it's `Subscribers` which
gives it the same ability to directly give `Messages` as the `Publisher`.
`Channels` also known as `Buses` can be connected to many `Publishers` and many `Subscribers`. The Detail Engine makes use of only
one `Channel` known as the `Message Bus` which links all the `Publishers` and `Subscribers`.
The `Channel` performs a check on the `Message Type` before sending it to the `Subscriber` as `Subscribers` can have specific type 
preferences so they don't get notified with `Messages` that they can't or otherwise don't need to work on.

### Subscribers
The `Subscriber` is an inheritable class that allows its Derived class to Receive `Messages`. It consists of two `Message` containers
that give it the ability to perform `Double Buffering` which is useful when `Systems` that Inherit `Subscriber` run on different
thread/s, as they need to perform a work on the `Messages` they Receive and working on them directly would block the `Message Bus` for
all other `Systems`, `Publishers` and `Subscribers` that either want to Send or Receive a `Message` from the `Bus`.
