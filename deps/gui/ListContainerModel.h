#pragma once

#include <giomm/listmodel.h>
#include <giomm/liststore.h>

namespace L4::Gui
{
    template <class T>
    class ListContainerModel : public Gio::ListModel, public Glib::Object
    {
        class WrapperItem;

        using ContainerT = std::remove_cvref_t<typename T>;
        using ValueT = ContainerT::value_type;

        class WrapperItem : public Glib::Object
        {
        public:
            WrapperItem(ValueT& Ref) :
                Ref(Ref)
            {
            }

            ValueT& Get() noexcept
            {
                return Ref;
            }

        private:
            ValueT& Ref;
        };

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
                return (new WrapperItem(Container[position]))->gobj();
            }
            return nullptr;
        }

    private:
        ContainerT& Container;
    };
}
