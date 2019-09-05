# AvoGUI
AvoGUI is a graphical application development library in c++ that is designed to be logical, structured and beautiful from the perspective of the user of the library. It gives the user as much control as possible without making him/her write unnecessary code. The library also has clear, readable and detailed documentation that answers most questions users would have about what things do and how they work. That is the vision of the project at least.

The project is inspired by Google's Material Design guidelines and includes some Material Design controls to help the user make good looking and feeling applications quickly.

## Getting started
All you need to do is add AvoGUI.hpp and AvoGUI.cpp to your project, and you're ready to start.

### Creating your GUI class
It is a good idea to create a class that inherits AvoGUI::GUI. The GUI is made up of a view tree structure, where every view has a parent, some have one or more children and some have one or more siblings. The GUI is the only view that doesn't have a parent, and it is the topmost view in the tree (depending on how you think about a tree, hmmm).

This is how your GUI can be implemented. Every method that can be overridden by your GUI (assuming it only inherits AvoGUI::GUI) is shown here.
```cpp
class MyApplication : public AvoGUI::GUI
{
  MyApplication()
  {
    // Here, you can call create(). This creates the window and drawing context that the GUI relies on.
    // It is recommended to do any other initialization in createContent(), which is called when the
    // drawing context and window are available.
  }
  ~MyApplication()
  {
    // Do not forget() views yourself; they are cleaned up automatically if they have a parent.
  }
  
  //------------------------------
  // Event handlers!
  
  void handleMouseDown(const AvoGUI::MouseEvent& p_event) override
  {
    // If you want to react to mouse down events from your GUI, you can do it here.
    // This is called from handleGlobalMouseDown, which is explained below. 
    // You need to call enableMouseEvents() for this handler to be called, since mouse events are disabled
    // by default. The mouse coordinates in the event are relative to the top-left corner of the GUI. 
    // This method is inherited from AvoGUI::View.
  }
  void handleGlobalMouseDown(const AvoGUI::MouseEvent& p_event) override
  {
    // This method sends a mouse down event to all targeted children - as well as the GUI.
    // It also sends out the global mouse events.
    // You could override this if you'd want your own algorithm for targeting the mouse events.
    // The most complicated method is handleGlobalMouseMove(), which sends enter, leave and move events.
    
    // Note that this method is called to the GUI directly from the window no matter what, so don't call 
    // addGlobalMouseEventListener() with a GUI as the parameter...
    // This method is inherited from AvoGUI::GlobalMouseEventListener.
  }
  
  // Other event listeners that you can override in your GUI are: handleMouseUp, handleMouseDoubleClick, 
  // handleMouseMove, handleMouseScroll, handleMouseEnter, handleMouseLeave, handleMouseBackgroundEnter,
  // handleMouseBackgroundLeave, handleCharacterInput, handleKeyboardKeyDown and handleKeyboardKeyUp - 
  // as well as the global versions of mouse events. Mouse events and keyboard events are explained more 
  // later in this readme.

  //------------------------------
  
  void createContent() override
  {
    // Here you can create your views, set up your theme and initialize other things. The purpose of having
    // this method is that resources like the window connected to the GUI and the drawing context used
    // for example to create text have been created and can be used. This method has no default implementation.
  }
  void handleSizeChange() override
  {
    // This is called when the GUI has changed size due to window resizing. This method has no default 
    // implementation. Note that this method belongs to the View class (which AvoGUI::GUI inherits) and can 
    // be implemented on any view. On the GUI, this will always be called after creation.
  }
  
  //------------------------------
  
  void draw(AvoGUI::DrawingContext* p_drawingContext) override
  {
    // Here you can draw directly in your GUI. It will appear behind child views. There is no default 
    // implementation.
  }
  void draw(AvoGUI::DrawingContext* p_drawingContext, const Rectangle<float>& p_targetRectangle) override
  {
    // By default, this method just calls the other draw method. You can use this one instead if you want to 
    // optimize drawing using the target rectangle, which is a rectangle specifying where drawing is needed 
    // (relative to the top-left corner of the GUI)
  }
  
  void drawOverlay(AvoGUI::DrawContext* p_drawingContext) override
  {
    // Here you can draw on top of child views. There is no default implementation.
    // Inherited from AvoGUI::View.
  }
  void drawOverlay(AvoGUI::DrawContext* p_drawingContext, const Rectangle<float>& p_targetRectangle) override
  {
    // By default, this method just calls the other drawOverlay method. 
    // Inherited from AvoGUI::View.
  }
}
```

### Creating a view
A view is a rectangle that can draw itself and react to different events. Views are used to create components and build up the GUI. They are also used as containers for other views, by setting them as the parent of the views that should be contained within it, using setParent() or the View constructor. Child views can only ever be drawn within their parent.

This is how a custom View class can be implemented. Every method that can be overridden by your view (assuming it only inherits AvoGUI::View) is shown here. 
```cpp
class MyView : public AvoGUI::View
{
  MyView(AvoGUI::View* p_parent, const AvoGUI::Rectangle<float>& p_bounds) : 
    // If you don't specify the bounds, it will be initialized with a size of (0, 0) and position of (0, 0).
    // You can then change the size and/or position afterwards if you want to.
    View(p_parent, p_bounds)
  {
    // Here you can initialize the view, add child views, do whatever you want. To add a child view, all 
    // you need to do is create a view with this view as the parent or change the parent of the view to this one. 
  }
  ~MyView()
  {
    // Do not forget() child views yourself. They are released automatically.
  }
  
  //------------------------------
  
  void updateAnimations() override
  {
    // This method is used when you want to animate things. It is never called more frequently than the 
    // monitor refresh rate. To add the view to the animation update queue, call queueAnimationUpdate(). 
    // If you would call queueAnimationUpdate() from this method every time, animations would be updated 
    // constantly at the refresh rate. This system is efficient since you only need to call 
    // queueAnimationUpdate() when you know that your animation isn't done.
  }
  
  void handleSizeChange() override
  {
    // This is called when the size of the view has changed. You can update your layout here. There is no 
    // standard implementation.
  }
  
  //------------------------------
  
  void handleMouseDown(const AvoGUI::MouseEvent& p_event) override
  {
    // Override this if you want to react to mouse button press events. You first need to call 
    // enableMouseEvents(), in the constructor or something. Otherwise, no mouse events will be recieved.
  }
  
  // Other mouse events that you can override by default in your view are: handleMouseUp, handleDoubleClick, 
  // handleMouseMove, handleMouseScroll, handleMouseLeave, handleMouseEnter, handleMouseBackgroundLeave and
  // handleMouseBackgroundEnter.
  
  //------------------------------
  
  void draw(AvoGUI::DrawingContext* p_drawingContext) override
  {
    // Draw your view here! If this view has children, do not draw them yourself. They are always drawn by 
    // the GUI, on top of what is drawn here. Now, how do you say that you want your view to get drawn? 
    // Do NOT call this method directly, instead call invalidate() or getGUI()->invalidateRect(...) to 
    // tell the system that a portion of the GUI has been updated and wants to be redrawn. Note that 
    // invalidate() cleans up if the view has been moved, by also invalidating the last rectangle.
    // invalidateRect(...) only makes sure one specific rectangle will be updated.
  }
  void draw(AvoGUI::DrawingContext* p_drawingContext, const AvoGUI::Rectangle<float>& p_targetRectangle) override
  {
    // This can be implemented instead of the other draw method if you want to know the rectangle that is going 
    // to be drawn. The default implementation only calls draw(AvoGUI::DrawingContext* p_drawingContext).
  }
  
  void drawOverlay(AvoGUI::DrawingContext* p_drawingContext) override
  {
    // Here, you can draw things on top of child views. 
  }
  void drawOverlay(AvoGUI::DrawingContext* p_drawingContext, const AvoGUI::Rectangle<float>& p_targetRectangle) override
  {
    // Default implementation only calls drawOverlay(AvoGUI::DrawingContext* p_drawingContext).
  }
}
```

#### Events
An event is something that happens. You often want your GUI components (views) to react to things that happen outside of their reach, for example a mouse click or when a child has resized itself. Views can inherit abstract classes which have *event handlers* you can override. The names of these abstract classes end with *Listener*, since they are objects that listen for events. The view also needs to be registered to the sender to recieve any events. The registering methods begin with "add" and end with the name of the event listener class. There are also unregistering methods which begin with "remove". You need to call that whenever a registered view is destroyed.

However, this is a bit different for a couple of event listeners.
To recieve keyboard events:
1. Let your class inherit AvoGUI::KeyboardListener.
2. Override the keyboard event handler methods you want to use.
3. Call getGUI()->setKeyboardFocus(view) when you want your view to be the target of keyboard events. This is because only 1 view can recieve keyboard events at a time. 

To respond to all keyboard events independent of being the keyboard focus:
1. Let your class inherit AvoGUI::KeyboardListener. There is no GlobalKeyboardListener class since that would be unnecessary.
2. Override the keyboard event handler methods you want to use.
3. Call getGUI()->addGlobalKeyboardListener(view) to register it to the GUI.

To make a view recieve mouse events, all you need to do is call enableMouseEvents() and override your handlers. This is because regular mouse events are targeted towards different views based on the view tree, and only views can be targeted this way.

To make an object recieve all mouse events that the window recieves from the OS, implement AvoGUI::GlobalMouseListener and register it with getGUI()->addGlobalMouseListener().

Look around in the documentation in AvoGUI.hpp for more information about anything!

## Compatible platforms
The goal is to support Windows, Linux and MacOS. Currently, only Windows is supported.

## Development state
The project is constantly morphing and taking shape to become the ideal GUI library, which means big changes can happen anytime as long as they improve for example workflow, consistency or efficiency. 

## Contributing
Don't be afraid to submit issues about the little things! All issues are very welcome, and they help the development of the library a lot. Pull requests are also welcome.
