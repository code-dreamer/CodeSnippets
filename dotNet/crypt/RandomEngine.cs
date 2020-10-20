
    public class RandomkEngine
    {
        public IEnumerable<int> Generate(int min, int max, int valuesCount)
        {
            var values = new List<int>(valuesCount);
            if (valuesCount == 0)
                return values;

            var rand = new MersenneTwisterGenerator();

            const int maxIterationCount = int.MaxValue;
            for (int i = 0; i < maxIterationCount && values.Count < valuesCount; ++i)
            {
                var val = (int)rand.Next((ulong)min, (ulong)max);
                if (!values.Contains(val))
                    values.Add(val);
            }

            if (values.Count != valuesCount)
                throw new Exception("Failed to generate values");

            return values;
        }
    }
