#pragma once

#include <giomm/listmodel.h>

namespace L4::Gui
{
    template <class T>
    class ListContainerModel : public Gio::ListModel, public Glib::Object
    {
        class WrapperItem;

        using ContainerT = typename std::remove_cvref_t<T>;
        using ValueT = typename ContainerT::value_type;
        using IteratorT = typename ContainerT::iterator;
        using RefT = Glib::RefPtr<WrapperItem>;
        using ConstRefT = Glib::RefPtr<const WrapperItem>;

    protected:
        ListContainerModel(ContainerT& Container) :
            Glib::ObjectBase(typeid(ListContainerModel)),
            Glib::Object(),
            Container(Container)
        {
        }

    public:
        static Glib::RefPtr<ListContainerModel> create(ContainerT& Container)
        {
            return Glib::make_refptr_for_instance<ListContainerModel>(new ListContainerModel(Container));
        }

        void items_changed(guint position, guint removed, guint added)
        {
            Gio::ListModel::items_changed(position, removed, added);
        }

        class WrapperItem : public Glib::Object
        {
        public:
            WrapperItem(IteratorT Itr) :
                Glib::ObjectBase(typeid(WrapperItem)),
                Glib::Object(),
                Itr(Itr)
            {
            }

        public:
            static Glib::RefPtr<WrapperItem> create(IteratorT Itr)
            {
                return Glib::make_refptr_for_instance<WrapperItem>(new WrapperItem(Itr));
            }

            const ValueT* operator->() const
            {
                return *Itr;
            }

            ValueT* operator->()
            {
                return *Itr;
            }

        private:
            IteratorT Itr;
        };

    protected:
        GType get_item_type_vfunc() override
        {
            return Glib::Object::get_base_type();
        }

        guint get_n_items_vfunc() override
        {
            return Container.size();
        }

        gpointer get_item_vfunc(guint position) override
        {
            if (position < Container.size())
            {
                return WrapperItem::create(Container.begin() + position)->gobj_copy();
            }
            return nullptr;
        }

    private:
        ContainerT& Container;
    };
}
