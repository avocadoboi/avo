# AvoGUI

AvoGUI is a graphical application development library in c++ that is designed to be logical, structured and beautiful from the perspective of the user of the library. It gives the user as much control as possible without making him/her write unnecessary code. The library also has clear, readable and detailed documentation that answers most questions users would have about what things do and how they work. That is the vision of the project at least.

The project is inspired by Google's Material Design guidelines and includes some Material Design controls to help the user make good looking and feeling applications quickly.

## Getting started

All you need to do is add "AvoGUI.hpp", "AvoGUI.cpp" and "Font data.hpp" to your project, and you're ready to start. c++17 standard is required.

### Creating your GUI class

It is a good idea to create a class that inherits AvoGUI::Gui. The GUI is made up of a view tree structure where every view has a parent, some have one or more children and some have one or more siblings. The GUI is the only view that doesn't have a parent view.

This is how your GUI can be implemented. Most methods that can be overridden by your GUI (assuming it only inherits AvoGUI::Gui) are shown here.

```cpp
class MyApplication : public AvoGUI::Gui
{
public:
  MyApplication()
  {
    /*
      Here, you can call create. This creates the window and drawing 
      context that the GUI relies on. After you are done initializing 
      the GUI, call run(). After that, the GUI is event driven.
    */
  }
  ~MyApplication()
  {
    /* 
      Do not forget() views yourself unless you have called remember 
      on them. forget is called automatically on views when they are 
      detached from their parent or when their parent destructs.
    */
  }

  //------------------------------
  // Event handlers!

  void handleMouseDown(AvoGUI::MouseEvent const&) override
  {
    /*
      If you want to react to mouse down events from your GUI, you can do 
      it here. You need to call View::enableMouseEvents for this handler 
      to be called, since mouse events are disabled by default. The mouse 
      coordinates in the event are relative to the top-left corner of 
      the GUI. This method is inherited from AvoGUI::View.
    */
  }

  /*
    There are many other event listeners available, and other ways to
    handle them. Mouse events and keyboard events are explained more later
    in this readme.
  */

  void handleSizeChange() override
  {
    /*
      This is called when the GUI has changed size due to window resizing.
      This method has no default implementation. Note that this method 
      belongs to the View class (which AvoGUI::GUI inherits) and can be 
      implemented on any view. On the GUI, this will always be called 
      after creation.
    */
  }

  //------------------------------

  void draw(AvoGUI::DrawingContext*) override
  {
    /*
      Here you can draw directly in your GUI. It will appear behind child
      views. There is no default implementation.
    */
  }
  void draw(AvoGUI::DrawingContext*, 
            AvoGUI::Rectangle<float> const&) override
  {
    /*
      By default, this method just calls the other draw method. You can 
      use this one instead if you want to optimize drawing using the 
      target rectangle, which is a rectangle specifying where drawing is 
      needed.
    */
  }

  void drawOverlay(AvoGUI::DrawContext*) override
  {
    /*
      Here you can draw on top of child views. There is no default 
      implementation. Inherited from AvoGUI::View.
    */
  }
  void drawOverlay(AvoGUI::DrawContext*, 
                   AvoGUI::Rectangle<float> const&) override
  {
    /*
      By default, this method just calls the other drawOverlay method. 
      Inherited from AvoGUI::View.
    */
  }
}
```

Don't forget that you might want to create a Gui object too, quite possibly in your entry point function! Views have to be dynamically allocated, but the root component isn't required to be since it doesn't have an owner.

### Creating a view

A view is a rectangle that can draw itself and react to different events. Views are used to create components and build up the GUI. They are also used as containers for other views, by setting them as the parent of the views that should be contained within it, using setParent() or the View constructor. Child views can only ever be drawn within their parent.

This is how a custom View class can be implemented. Most methods that can be overridden by your view (assuming it only inherits AvoGUI::View) are shown here. 

```cpp
class MyView : public AvoGUI::View
{
public:
  MyView(AvoGUI::View* p_parent, AvoGUI::Rectangle<float> const& p_bounds) : 
    /* 
      If you don't specify the bounds, it will be initialized with a 
      size of (0, 0) and position of (0, 0). You can then change the 
      size and/or position afterwards if you want to.
    */
    View(p_parent, p_bounds)
  {
    /*
      Here you can initialize the view, add child views, do whatever 
      you want. To add a child view, all you need to do is create a view 
      with this view as the parent or change the parent of the view to 
      this one. 
    */
  }
  ~MyView()
  {
    /*
      Rarely needed. All memory created with the framework is managed 
      by the framework!
    */
  }

  //------------------------------

  void updateAnimations() override
  {
    /*
      This method is used when you want to animate things. It is never 
      called more frequently than the monitor refresh rate. To add the 
      view to the animation update queue, call queueAnimationUpdate().
    */
  }

  void handleSizeChange() override
  {
    /*
      This is called when the size of the view has changed. You could
      update your layout here. There is no standard implementation.
    */
  }

  //------------------------------

  void draw(AvoGUI::DrawingContext*) override
  {
    /*
      Draw your view here! If this view has children, do not draw them 
      yourself. They are always drawn by the GUI, on top of what is drawn 
      here. Now, how do you say that you want your view to get drawn? 
      Do NOT call this method directly, instead call View::invalidate or 
      Gui::invalidateRect to tell the system that a portion of the GUI has
      been updated and wants to be redrawn. Note that invalidate() cleans
      up if the view has been moved, by also invalidating the last 
      rectangle. invalidateRect(...) only makes sure one specific 
      rectangle will be updated.
    */
  }
  void draw(AvoGUI::DrawingContext*, 
            AvoGUI::Rectangle<float> const&) override
  {
    /*
      This can be implemented instead of the other draw method if you want 
      to know the rectangle that is going to be drawn. The default 
      implementation only calls View::draw(AvoGUI::DrawingContext*).
    */
  }

  void drawOverlay(AvoGUI::DrawingContext*) override
  {
    // Here, you can draw things on top of child views. 
  }
  void drawOverlay(AvoGUI::DrawingContext*, 
                   AvoGUI::Rectangle<float> const&) override
  {
    /*
      Default implementation only calls 
      drawOverlay(AvoGUI::DrawingContext*).
    */
  }
}
```

### Events

An event is something that happens. You often want your GUI components (views) to react to things that happen outside of their reach, for example a mouse click or when a child has resized itself. Views have a couple of built-in event listeners and some of these need to be activated in different ways. For all events in the framework, additional listener callbacks can be attached in the form of any callable type.

Example with a lambda:

```cpp
child->sizeChangeListeners += [this](View*, float, float) { /* Do anything! */ };
```

Example with a member function:

```cpp
child->sizeChangeListeners += AvoGUI::bind(&MyView::doSomething, this);
```

To receive keyboard events:

1. Override the keyboard event handler methods you want to use.
2. Call getGui()->setKeyboardFocus(view) when you want your view to be the target of keyboard events. This is because only 1 view can receive keyboard events at a time. 

To respond to all keyboard events independent of being the keyboard focus, you can add global keyboard event callbacks using Gui::globalKeyboardKeyDownListeners as an example.

To make a view receive mouse events, all you need to do is call enableMouseEvents() and override your handlers. To add an additional listener to a view, do as usual and use View::mouseDownListeners for example.

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
