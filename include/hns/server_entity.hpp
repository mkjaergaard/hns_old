

#include <map>
#include <set>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/tokenizer.hpp>

namespace hns
{

class Namespace;
class Tag;
class PseudoTag;


class MyCallback
{
public:
  virtual void tagEntryAdded(const boost::uuids::uuid& tag_id,
			     const boost::uuids::uuid& alias_id,
			     const boost::uuids::uuid& entity_id) = 0;
  virtual void tagEntryRemoved(const boost::uuids::uuid& tag_id,
			       const boost::uuids::uuid& entity_id) = 0;

};

class Namespace
{
protected:
  typedef std::set<boost::uuids::uuid> IDListType;

  IDListType node_list_;
  IDListType child_namespace_list_;
  IDListType tag_list_;
  IDListType pseudo_tag_list_;

  boost::uuids::uuid id_;
  boost::uuids::uuid parent_id_;

  std::string name_;

public:
  Namespace(const boost::uuids::uuid& id,
	    const boost::uuids::uuid& parent_id,
	    const std::string& name) :
    id_(id),
    parent_id_(parent_id),
    name_(name)
  {
  }

  IDListType& accessChildNamespaceList()
  {
    return child_namespace_list_;
  }

  const IDListType& accessChildNamespaceList() const
  {
    return child_namespace_list_;
  }

  IDListType& accessTagList()
  {
    return tag_list_;
  }

  const IDListType& accessTagList() const
  {
    return tag_list_;
  }

  IDListType& accessPseudoTagList()
  {
    return tag_list_;
  }

  const IDListType& accessPseudoTagList() const
  {
    return tag_list_;
  }

  const std::string& getName() const
  {
    return name_;
  }

  const boost::uuids::uuid& getID()
  {
    return id_;
  }

  const boost::uuids::uuid& getParentNamespace()
  {
    return parent_id_;
  }

};

class Tag
{
protected:
  typedef std::set<boost::uuids::uuid> NodeListType;

  std::string name_;
  boost::uuids::uuid id_;
  NodeListType node_list_;

  boost::uuids::uuid namespace_id_;

  size_t users_;

  typedef std::vector<MyCallback*> SubscriberListType;
  SubscriberListType subscribers_;

public:
  Tag(const boost::uuids::uuid& id, const boost::uuids::uuid& namespace_id, const std::string& name) :
    id_(id),
    namespace_id_(namespace_id),
    name_(name)
  {
  }

  const std::string& getName() const
  {
    return name_;
  }

  const boost::uuids::uuid& getNamespace()
  {
    return namespace_id_;
  }

  void addSubscriber(MyCallback* callback)
  {
    subscribers_.push_back(callback);
  }

  void triggerSubscribers(const boost::uuids::uuid& tag_id,
			  const boost::uuids::uuid& alias_id,
			  const boost::uuids::uuid& entity_id)
  {
    for(SubscriberListType::iterator it = subscribers_.begin();
	it != subscribers_.end();
	it++)
    {
      (*it)->tagEntryAdded(tag_id, alias_id, entity_id);
    }
  }

};

class PseudoTag
{
protected:
  std::string name_;
  boost::uuids::uuid id_;
  boost::uuids::uuid namespace_id_;

  boost::uuids::uuid target_namespace_id_;
  std::string target_name_;
  // peger på?

public:
  PseudoTag(const boost::uuids::uuid& id,
	    const boost::uuids::uuid& namespace_id,
	    const std::string& name,
	    const boost::uuids::uuid& target_namespace_id,
	    const std::string& target_name) :
    id_(id),
    namespace_id_(namespace_id),
    name_(name),
    target_namespace_id_(target_namespace_id),
    target_name_(target_name)
  {
  }

  const std::string& getName() const
  {
    return name_;
  }

  const std::string& getTargetName() const
  {
    return target_name_;
  }

  const boost::uuids::uuid& getTargetNamespace() const
  {
    return target_namespace_id_;
  }


};

class ServerEntity
{
protected:
  typedef std::set<boost::uuids::uuid> IDListType;

  typedef std::map<boost::uuids::uuid, boost::shared_ptr<Namespace> > NamespaceListType;
  typedef std::map<boost::uuids::uuid, boost::shared_ptr<Tag> > TagListType;
  typedef std::map<boost::uuids::uuid, boost::shared_ptr<PseudoTag> > PseudoTagListType;

  NamespaceListType namespace_list_;
  TagListType tag_list_;
  PseudoTagListType pseudo_tag_list_;

  // todo: use something else
  boost::uuids::random_generator id_generator_;

  boost::uuids::uuid current_ns_id_;

  // todo: use something else
  boost::uuids::uuid nil_;

  // Tokenized stuf
  typedef boost::tokenizer<boost::char_separator<char> > TokenizerType;
  boost::char_separator<char> seperator_;

public:
  ServerEntity() :
    nil_(boost::uuids::nil_generator()()),
    current_ns_id_(id_generator_()),
    seperator_("/", "", boost::keep_empty_tokens)
  {
    namespace_list_.insert(NamespaceListType::value_type(current_ns_id_,
							 boost::make_shared<Namespace>(current_ns_id_,
										       nil_,
										       ".") ));
  }

  boost::shared_ptr<Tag>& findTag(const boost::uuids::uuid& tag_id)
  {
    TagListType::iterator item = tag_list_.find(tag_id);
    assert(item != tag_list_.end());
    return item->second;
  }

  boost::shared_ptr<PseudoTag>& findPseudoTag(const boost::uuids::uuid& pseudo_tag_id)
  {
    PseudoTagListType::iterator item = pseudo_tag_list_.find(pseudo_tag_id);
    assert(item != pseudo_tag_list_.end());
    return item->second;
  }

  boost::shared_ptr<Namespace>& findNamespace(const boost::uuids::uuid& ns_id)
  {
    NamespaceListType::iterator item = namespace_list_.find(ns_id);
    assert(item != namespace_list_.end());
    return item->second;
  }

  void populateGroupFindLocalPseudoTags(const std::string& target_tag_name,
					const boost::shared_ptr<Namespace> target_ns,
					std::set<boost::uuids::uuid>& group)
  {
    for(IDListType::const_iterator it = target_ns->accessPseudoTagList().begin();
	it != target_ns->accessPseudoTagList().end();
	it++)
    {
      boost::shared_ptr<PseudoTag> pseudo_tag = findPseudoTag(*it);
      if (pseudo_tag->getTargetName() == target_tag_name)
      {
	populateGroupFromTagName(pseudo_tag->getName(), target_ns, group);
      }
      else if(pseudo_tag->getName() == target_tag_name)
      {
	populateGroupFromTagName(pseudo_tag->getTargetName(),
				 findNamespace(pseudo_tag->getTargetNamespace()),
				 group);
      }
    }
  }

  void populateGroupFindParentPseudoTags(const std::string& target_tag_name,
					 const boost::shared_ptr<Namespace> target_ns,
					 std::set<boost::uuids::uuid>& group)
  {
    // 3 - search for remappings in parent namespace.
    if(target_ns->getParentNamespace() == nil_)
    {
      std::cout << "Not searching entity parent namespace for pseudo tags" << std::endl;
    }
    else
    {
      boost::shared_ptr<Namespace> parent_ns = findNamespace(target_ns->getParentNamespace());
      for(IDListType::const_iterator it = parent_ns->accessPseudoTagList().begin();
	  it != parent_ns->accessPseudoTagList().end();
	  it++)
      {
	boost::shared_ptr<PseudoTag> pseudo_tag = findPseudoTag(*it);
	if(pseudo_tag->getTargetNamespace() == target_ns->getID() &&
	   pseudo_tag->getTargetName() == target_tag_name)
	{
	  // match!
	  populateGroupFromTagName(pseudo_tag->getName(), parent_ns, group);
	}
      }
    }
  }

// based on tag
  void populateGroupFromTagName(const std::string& tag_name,
				const boost::shared_ptr<Namespace> ns,
				std::set<boost::uuids::uuid>& group)
  {
    // 2 - search for tags with same name
    for(IDListType::const_iterator it = ns->accessTagList().begin();
	it != ns->accessTagList().end();
	it++)
    {
      boost::shared_ptr<Tag> child_tag = findTag(*it);
      if(child_tag->getName() == tag_name)
      {
	if(group.find(*it) == group.end())
	{
	  group.insert(*it);
	}
	else
	{
	  return; // allready visited, bail out
	}
      }
    }
    populateGroupFindLocalPseudoTags(tag_name, ns, group);
    populateGroupFindParentPseudoTags(tag_name, ns, group);
  }

  void populateGroupFromTag(const boost::uuids::uuid& tag_id,
			    std::set<boost::uuids::uuid>& group)
  {
    boost::shared_ptr<Tag> tag = findTag(tag_id);
    populateGroupFromTagName(tag->getName(),
			     findNamespace(tag->getNamespace()),
			     group);
  }

  void populateGroup(boost::uuids::uuid tag_i, std::set<boost::uuids::uuid> group)
  {
    group.clear();
    // 1 - find all tags with same name
    // 2 - find all pseudo-tags in parent namespace
    // 3 - recurse
  }


  // not thread safe, only call from Server thread
  const boost::uuids::uuid& hasChildNamespace(const boost::shared_ptr<const Namespace>& ns, const std::string& child_name)
  {
    for(IDListType::const_iterator it = ns->accessChildNamespaceList().begin();
	it != ns->accessChildNamespaceList().end();
	it++)
    {
      const boost::uuids::uuid& child_ns_id = *it;
      NamespaceListType::iterator item = namespace_list_.find(child_ns_id);
      assert( item != namespace_list_.end() );

      if(item->second->getName() == child_name)
      {
	return child_ns_id;
      }
    }
    return nil_;
  }

  // todo: collect these in a templated function
  const boost::uuids::uuid& hasTag(const boost::shared_ptr<const Namespace>& ns, const std::string& tag_name)
  {
    for(IDListType::const_iterator it = ns->accessTagList().begin();
	it != ns->accessTagList().end();
	it++)
    {
      const boost::uuids::uuid& tag_id = *it;
      TagListType::iterator item = tag_list_.find(tag_id);
      assert( item != tag_list_.end() );

      if(item->second->getName() == tag_name)
      {
	return tag_id;
      }
    }
    return nil_;
  }

  // todo: collect these in a templated function
  const boost::uuids::uuid& hasPseudoTag(const boost::shared_ptr<const Namespace>& ns, const std::string& tag_name)
  {
    for(IDListType::const_iterator it = ns->accessTagList().begin();
	it != ns->accessTagList().end();
	it++)
    {
      const boost::uuids::uuid& tag_id = *it;
      PseudoTagListType::iterator item = pseudo_tag_list_.find(tag_id);
      assert( item != pseudo_tag_list_.end() );

      if(item->second->getName() == tag_name)
      {
	return tag_id;
      }
    }
    return nil_;
  }

  const boost::uuids::uuid& registerNamespace(const boost::uuids::uuid& parent_ns_id, const std::string& name)
  {
    boost::shared_ptr<Namespace>& parent = findNamespace(parent_ns_id);

    const boost::uuids::uuid& child_id = hasChildNamespace(parent, name);

    if(child_id == nil_)
    {
      boost::uuids::uuid new_id = id_generator_();
      boost::shared_ptr<Namespace> ns = boost::make_shared<Namespace>(new_id, parent_ns_id, name);
      parent->accessChildNamespaceList().insert(new_id);
      NamespaceListType::iterator new_item = namespace_list_.insert(NamespaceListType::value_type(new_id, ns)).first;
      std::cout << "Adding new Namespace " << name << std::endl;
      return new_item->first;
    }
    else
    {
      std::cout << "Namespace Already Exists " << name << std::endl;
      return child_id;
    }
  }

  // based on correct parent ns id
  const boost::uuids::uuid& registerPseudoTag(const boost::uuids::uuid& parent_ns_id,
					      const std::string& name,
					      const boost::uuids::uuid& target_ns_id,
					      const std::string& target_name)
  {
    boost::shared_ptr<Namespace>& parent_ns = findNamespace(parent_ns_id);

    //todo: manage duplicate tags

    boost::uuids::uuid new_id = id_generator_();
    boost::shared_ptr<PseudoTag> tag = boost::make_shared<PseudoTag>(new_id,
								     parent_ns_id,
								     name,
								     target_ns_id,
								     target_name);
    parent_ns->accessPseudoTagList().insert(new_id);
    PseudoTagListType::iterator new_item = pseudo_tag_list_.insert(PseudoTagListType::value_type(new_id, tag)).first;
    std::cout << "Adding new Pseudo Tag " << name << " -> " << target_name << std::endl;
    return new_item->first;
  }

  const boost::uuids::uuid& registerTag(const boost::uuids::uuid& parent_ns_id, const std::string& name)
  {
    //todo: collect in function
    boost::shared_ptr<Namespace>& parent = findNamespace(parent_ns_id);

    const boost::uuids::uuid& tag_id = hasTag(parent, name);

    if(tag_id == nil_)
    {
      boost::uuids::uuid new_id = id_generator_();
      boost::shared_ptr<Tag> tag = boost::make_shared<Tag>(new_id, parent_ns_id, name);
      parent->accessTagList().insert(new_id);
      TagListType::iterator new_item = tag_list_.insert(TagListType::value_type(new_id, tag)).first;
      std::cout << "Adding new Tag " << name << std::endl;
      return new_item->first;
    }
    else
    {
      std::cout << "Tag Already Exists " << name << std::endl;
      return tag_id;
    }
  }

  // based on full string
  const boost::uuids::uuid& registerTag(const std::string& tag)
  {

    boost::uuids::uuid current_ns_id = current_ns_id_;

    //Tokenizer stuff, move to generic place
    TokenizerType tokenizer(tag, seperator_);
    TokenizerType::iterator item = tokenizer.begin();
    assert(item != tokenizer.end()); // throw invalid ns exception
    TokenizerType::iterator last = item;
    item++;
    while(item != tokenizer.end())
    {
      assert(*last != ""); // proper checking + throw invalid ns exception
      std::cout << "NS:" << *last << std::endl;
      //
      current_ns_id = registerNamespace(current_ns_id, *last);
      //
      last = item;
      item++;
    }
    assert(*last != ""); // proper checking + throw invalid ns exception
    std::cout << "Tag:" << *last << std::endl;
    //Tonenizer stuff - end

    return registerTag(current_ns_id, *last);
  }

  void subscribeTag(const std::string& tag, MyCallback* callback)
  {
    const boost::uuids::uuid& tag_id = registerTag(tag);

    boost::shared_ptr<Tag>& tag_entry = findTag(tag_id);

    //tag_entry->addSubscriber(callback);
    std::set<boost::uuids::uuid> group;
    populateGroupFromTag(tag_id, group);
    std::cout << "Size of group " << group.size() << std::endl;
  }

  const boost::uuids::uuid& registerPseudoTag(const std::string& tag, const std::string& target_tag)
  {
    //////
    // Handle the tag name
    boost::uuids::uuid current_ns_id = current_ns_id_;

    //Tokenizer stuff, move to generic place
    TokenizerType tokenizer(tag, seperator_);
    TokenizerType::iterator item = tokenizer.begin();
    assert(item != tokenizer.end()); // throw invalid ns exception
    TokenizerType::iterator last = item;
    item++;
    while(item != tokenizer.end())
    {
      assert(*last != ""); // proper checking + throw invalid ns exception
      std::cout << "NS:" << *last << std::endl;
      //
      current_ns_id = registerNamespace(current_ns_id, *last);
      //
      last = item;
      item++;
    }
    assert(*last != ""); // proper checking + throw invalid ns exception
    std::cout << "Pseudo Tag:" << *last << std::endl;
    //Tonenizer stuff - end

    ///////
    // Target tag:
    TokenizerType tokenizer2(target_tag, seperator_);
    item = tokenizer2.begin();
    assert(item != tokenizer2.end()); // throw invalid ns exception

    boost::uuids::uuid target_ns_id = current_ns_id;
    std::string target_tag_name = *item;
    item++;
    if(item != tokenizer2.end())
    {
      target_ns_id = registerNamespace(current_ns_id, target_tag);
      std::string target_tag_name = *item;
    }

    return registerPseudoTag(current_ns_id, *last, target_ns_id, target_tag_name);
  }

  // callback ved alias tags

};

}
