#pragma once
#include "maths.h"
#include <algorithm>
#include <memory>
#include <set>
#include <typeindex>
#include <vector>

class Entity;
class Scene;

class Component {
  friend Entity;

protected:
  Entity* const _parent;
  bool _fordeletion; // should be removed
  explicit Component(Entity* const p);

public:
  Component() = delete;

  bool is_fordeletion() const;

  virtual void update(double dt) = 0;

  virtual void render() = 0;

  virtual ~Component();
};

struct EntityManager {
  std::vector<std::shared_ptr<Entity>> list;
  void update(double dt);
  void render();
  std::vector<std::shared_ptr<Entity>> find(const std::string& tag) const;
  std::vector<std::shared_ptr<Entity>>
  find(const std::vector<std::string>& tags) const;
};

class Entity {
  friend struct EntityManager;

protected:
  std::vector<std::shared_ptr<Component>> _components;
  sf::Vector2f _position;
  float _rotation;
  bool _alive;       // should be updated
  bool _visible;     // should be rendered
  bool _fordeletion; // should be deleted
  std::set<std::string> _tags;

public:
  void addTag(const std::string& t);
  const std::set<std::string>& getTags() const;
  Scene* const scene;
  Entity(Scene* const s);

  virtual ~Entity();

  virtual void update(double dt);

  virtual void render();

  //
  const sf::Vector2f& getPosition() const;

  void setPosition(const sf::Vector2f& _position);

  bool is_fordeletion() const;

  float getRotation() const;

  void setRotation(float _rotation);

  bool isAlive() const;

  void setAlive(bool _alive);

  void setForDelete();

  bool isVisible() const;

  void setVisible(bool _visible);

  template <typename T, typename... Targs>
  std::shared_ptr<T> addComponent(Targs... params) {
    static_assert(std::is_base_of<Component, T>::value, "T != component");
    std::shared_ptr<T> sp(std::make_shared<T>(this, params...));
    _components.push_back(sp);
    return sp;
  }

  template <typename T>
  const std::vector<std::shared_ptr<T>> get_components() const {
    static_assert(std::is_base_of<Component, T>::value, "T != component");
    std::vector<std::shared_ptr<T>> ret;
    for (const auto c : _components) {
      //if (typeid(*c) == typeid(T)) { // on this line I'm getting a warning:  warning: expression with side effects will be evaluated despite being used as an operand to 'typeid' [-Wpotentially-evaluated-expression]. What is this warning about? A: This is a warning about the fact that you are using a variable in a way that has side effects, and then using the result of that in a way that does not have side effects. The compiler is warning you that it will evaluate the expression with side effects, even though the result is not used. This is a warning because it is often a mistake. For example, if you have a function that returns a pointer, and you want to check whether the pointer is null, you might write if (typeid(f()) == typeid(void*)) { ... }. This will call f() even if the pointer is null, which is probably not what you want. The correct way to write this is if (auto p = f(); typeid(p) == typeid(void*)) { ... }.
        //what is a better way of writing the previous line if (typeid(*c) == typeid(T))? A: You can use dynamic_cast instead of typeid. dynamic_cast will return nullptr if the cast fails, so you can write if (dynamic_cast<T*>(c)) { ... }.
          if (dynamic_cast<T*>(c)) { // this is the line I changed
          ret.push_back(std::dynamic_pointer_cast<T>(c));
      }
    }
    return std::move(ret);
  }

  // Will return a T component, or anything derived from a T component.
  template <typename T>
  const std::vector<std::shared_ptr<T>> GetCompatibleComponent() {
    static_assert(std::is_base_of<Component, T>::value, "T != component");
    std::vector<std::shared_ptr<T>> ret;
    for (auto c : _components) {
      auto dd = dynamic_cast<T*>(&(*c));
      if (dd) {
        ret.push_back(std::dynamic_pointer_cast<T>(c));
      }
    }
    return ret;
  }
};
