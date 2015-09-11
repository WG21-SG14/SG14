using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace cs_func_sort
{
    class A
    {
        virtual public void Foo()
        {
            Console.WriteLine("A::Foo");
        }
        virtual public void Name()
        {
            Console.Write("Class A ");
        }
    }
    class B : A
    {
        override public void Foo()
        {
            Console.WriteLine("B::Foo");
        }
        override public void Name()
        {
            Console.Write("Class B ");
        }
    }

    class C : B
    {
        override public void Name()
        {
            Console.Write("Class C ");
        }
    }

    delegate void fptrDelegate();
    class Program
    {
        static void Main(string[] args)
        {
            List<A> list = new List<A>();
            list.Add(new A());
            list.Add(new C());
            list.Add(new B());
            list.Add(new A());
            list.Add(new C());

            Console.WriteLine("Print the list unsorted");
            foreach (A cur in list)
            {
                cur.Name();
                cur.Foo();
            }


            Console.WriteLine("Print the list sorted");
            list.Sort(delegate (A itemA, A itemB)
            {
                fptrDelegate afoo = itemA.Foo;
                fptrDelegate bfoo = itemB.Foo;
                return afoo.Method.GetHashCode() - bfoo.Method.GetHashCode();
            });

            foreach (A cur in list)
            {
                cur.Name();
                cur.Foo();
            }

            Console.WriteLine("Find all of the version with B");
            // Can't think of a good way to get rid of creating this object in C#
            A testA = new B();
            fptrDelegate testfoo = testA.Foo;
            foreach (A cur in list)
            {
                fptrDelegate curfoo = cur.Foo;
                if( testfoo.Method.Equals(curfoo.Method) )
                {
                    // call B::Foo AND C::Foo!! 
                    cur.Name();
                    cur.Foo();
                }
            }
        }
    }
}
