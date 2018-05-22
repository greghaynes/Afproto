using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Afproto;

namespace ExampleApp
{
    class Program
    {
        static void Main(string[] args)
        {
            var data = Encoding.ASCII.GetBytes("Test");

            var framedData = Afproto.Framer.FrameData(data);

            int number = 0;
            var deFramedData = Afproto.Deframer.getData(framedData, ref number);

            var output = Encoding.ASCII.GetString(deFramedData);
            Console.WriteLine("Data Parsed: " + output);
            Console.WriteLine("Bytes to be removed from buffer: " + number.ToString());
            Console.ReadKey();
        }
    }
}
