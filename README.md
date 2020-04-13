# AvoGUI
AvoGUI is a graphical application development library in c++ that is designed to be logical, structured and beautiful from the perspective of the user of the library. It gives the user as much control as possible without making him/her write unnecessary code. The library also has clear, readable and detailed documentation that answers most questions users would have about what things do and how they work. That is the vision of the project at least.

The project is inspired by Google's Material Design guidelines and includes some Material Design controls to help the user make good looking and feeling applications quickly.

## Getting started
All you need to do is add "AvoGUI.hpp", "AvoGUI.cpp" and "Font data.hpp" to your project, and you're ready to start.

### Creating your GUI class
It is a good idea to create a class that inherits AvoGUI::Gui. The GUI is made up of a view tree structure, where every view has a parent, some have one or more children and some have one or more siblings. The GUI is the only view that doesn't have a parent, and it is the topmost view in the tree (depending on how you think about a tree, hmmm).

This is how your GUI can be implemented. Most methods that can be overridden by your GUI (assuming it only inherits AvoGUI::Gui) are shown here.
```cpp
class MyApplication : public AvoGUI::Gui
{
public:
  MyApplication()
  {
    // Here, you can call create(). This creates the window and drawing context that the GUI relies on.
    // It is recommended to do any other initialization in createContent(), which is called when the
    // drawing context and window are available. You could also call waitForFinish() here, to wait for 
    // the application to close.
  }
  ~MyApplication()
  {
    // Do not forget() views yourself unless you have called remember() on them. 
    // forget is called automatically on views when they are detached from their parent or when their parent destructs.
  }
  
  //------------------------------
  // Event handlers!
  
  void handleMouseDown(AvoGUI::MouseEvent const& p_event) override
  {
    // If you want to react to mouse down events from your GUI, you can do it here.
    // You need to call enableMouseEvents() for this handler to be called, since mouse events are disabled
    // by default. The mouse coordinates in the event are relative to the top-left corner of the GUI. 
    // This method is inherited from AvoGUI::View.
  }
  
  // Other event listeners that you can override in your GUI are: handleMouseUp, handleMouseDoubleClick, 
  // handleMouseMove, handleMouseScroll, handleMouseEnter, handleMouseLeave, handleMouseBackgroundEnter,
  // handleMouseBackgroundLeave, handleCharacterInput, handleKeyboardKeyDown, handleKeyboardKeyUp. 
  // Mouse events and keyboard events are explained more later in this readme.

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
  void draw(AvoGUI::DrawingContext* p_drawingContext, AvoGUI::Rectangle<float> const& p_targetRectangle) override
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
  void drawOverlay(AvoGUI::DrawContext* p_drawingContext, AvoGUI::Rectangle<float> const& p_targetRectangle) override
  {
    // By default, this method just calls the other drawOverlay method. 
    // Inherited from AvoGUI::View.
  }
}
```
Don't forget that you might want to create a Gui object too, quite possibly in your entry point function! Views and GUIs have to be dynamically allocated (as with all other classes that inherit ReferenceCounted).

### Creating a view
A view is a rectangle that can draw itself and react to different events. Views are used to create components and build up the GUI. They are also used as containers for other views, by setting them as the parent of the views that should be contained within it, using setParent() or the View constructor. Child views can only ever be drawn within their parent.

This is how a custom View class can be implemented. Most methods that can be overridden by your view (assuming it only inherits AvoGUI::View) are shown here. 
```cpp
class MyView : public AvoGUI::View
{
public:
  MyView(AvoGUI::View* p_parent, AvoGUI::Rectangle<float> const& p_bounds) : 
    // If you don't specify the bounds, it will be initialized with a size of (0, 0) and position of (0, 0).
    // You can then change the size and/or position afterwards if you want to.
    View(p_parent, p_bounds)
  {
    // Here you can initialize the view, add child views, do whatever you want. To add a child view, all 
    // you need to do is create a view with this view as the parent or change the parent of the view to this one. 
  }
  ~MyView()
  {
    // Release resources like drawable images and text by calling forget on them.
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
  
  void handleMouseDown(AvoGUI::MouseEvent const& p_event) override
  {
    // Override this if you want to react to mouse button press events. You first need to call 
    // enableMouseEvents(), in the constructor or something. Otherwise, no mouse events will be received.
    // The mouse coordinates in the event are relative to the top left corner of this view.
  }
  
  // You can override the same listeners as your GUI can.
  
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
  void draw(AvoGUI::DrawingContext* p_drawingContext, AvoGUI::Rectangle<float> const& p_targetRectangle) override
  {
    // This can be implemented instead of the other draw method if you want to know the rectangle that is going 
    // to be drawn. The default implementation only calls draw(AvoGUI::DrawingContext* p_drawingContext).
  }
  
  void drawOverlay(AvoGUI::DrawingContext* p_drawingContext) override
  {
    // Here, you can draw things on top of child views. 
  }
  void drawOverlay(AvoGUI::DrawingContext* p_drawingContext, AvoGUI::Rectangle<float> const& p_targetRectangle) override
  {
    // Default implementation only calls drawOverlay(AvoGUI::DrawingContext* p_drawingContext).
  }
}
```

### Events
An event is something that happens. You often want your GUI components (views) to react to things that happen outside of their reach, for example a mouse click or when a child has resized itself. Views have a couple of built-in event listeners and some of these need to be activated in different ways. For all events in the framework, additional listener callbacks can be attached in the form of any callable type.

Example with a lambda:
```cpp
child->addSizeChangeListener([this](View*, float, float) { /* Do anything! */ });
```
Example with a member function:
```cpp
child->addSizeChangeListener(AvoGUI::bind(&MyView::doSomething, this));
```

To receive keyboard events:
1. Override the keyboard event handler methods you want to use.
2. Call getGui()->setKeyboardFocus(view) when you want your view to be the target of keyboard events. This is because only 1 view can receive keyboard events at a time. 

To respond to all keyboard events independent of being the keyboard focus, you can add global keyboard event callbacks using the Gui::addGlobalKeyboardKeyDownListener method as an example.

To make a view receive mouse events, all you need to do is call enableMouseEvents() and override your handlers. To add an additional listener to a view, do as usual and call View::addMouseDownListener for example.

### Unicode characters
AvoGUI supports non-ASCII UTF-8 unicode characters. To create a UTF-8 encoded string literal, simply prepend the "u8" prefix. Example: u8"Göran matar bläckfiskarna med blått bläck.". 

To support right-to-left languages, set the reading direction of the text to ReadingDirection::RightToLeft. You can set the default reading direction by modifying the default TextProperties in the DrawingContext.

Look around in the documentation in AvoGUI.hpp for more information about anything!

## Compatible platforms
The goal is to support Windows, Linux and MacOS. Currently, only Windows is fully supported and Linux development is in progress.

## Development state
The project is constantly morphing and taking shape to become the ideal GUI library, which means big changes can happen anytime as long as they improve for example workflow, consistency or efficiency. However, the latest version on the master branch should be pretty stable at this point.

## Contributing
Don't be afraid to submit issues about the little things! All issues are very welcome, and they help the development of the library a lot. Pull requests are also welcome.
