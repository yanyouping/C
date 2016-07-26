using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Oid
{
    class Program
    {
        static void Main(string[] args)
        {
            const int k = 400000;
            Util.ObjectId[] r = new Util.ObjectId[k];
            for (int i = 0; i < k; i++)
            {
                r[i] = Util.ObjectId.GenerateNewId();
            }
            for (int i = 0; i < (int)(k*0.250); i++)
            {
                Console.WriteLine($"{r[i * 4 + 0].ToString()}    {r[i * 4 + 1].ToString()}    {r[i * 4 + 2].ToString()}    {r[i * 4 + 3].ToString()}");
            }
            Console.ReadLine();
        }
    }
}
