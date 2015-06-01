/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

#include "Probes.h"

namespace OODebug {

QPair<QList<Probes::ValueCalculator>, QStringList> Probes::parseProbeArguments(QStringList arguments)
{

	static const QRegularExpression NUMBER_REGEX{"^(\\d+)$"};
	static const QRegularExpression OPERATOR_REGEX{"^([+\\-*/]?)$"};

	QList<ValueCalculator> calculators;
	QStringList variableNames;
	for (int i = 0; i < arguments.size(); ++i)
	{
		QString arg = arguments[i];
		if (!OPERATOR_REGEX.match(arg).hasMatch())
		{
			bool isNumber = NUMBER_REGEX.match(arg).hasMatch();
			double value = 0;
			if (isNumber) value = arg.toDouble();
			int index = variableNames.indexOf(arg);
			if (!isNumber && -1 == index)
			{
				index = variableNames.size();
				variableNames << arg;
			}
			// lookahead
			if (i + 1 < arguments.size() && OPERATOR_REGEX.match(arguments[i+1]).hasMatch())
			{
				// TODO: If after a operator we do not have anything we just fail here:
				if (i + 2 >= arguments.size()) Q_ASSERT(false);

				auto opFunc = operatorFromString(arguments[i+1]);

				bool isNumber2 = NUMBER_REGEX.match(arguments[i+2]).hasMatch();
				double value2 = 0;
				if (isNumber2) value2 = arguments[i+2].toDouble();
				int index2 = variableNames.indexOf(arguments[i+2]);
				if (!isNumber2 && -1 == index2)
				{
					index2 = variableNames.size();
					variableNames << arguments[i+2];
				}
				if (isNumber && isNumber2)
					calculators << [value, value2, opFunc](QList<double>)
						{return opFunc(value, value2);};
				else if (isNumber)
					calculators <<  [value, index2, opFunc](QList<double> values)
						{return opFunc(value, values[index2]);};
				else if (isNumber2)
					calculators <<  [index, value2, opFunc](QList<double> values)
						{return opFunc(values[index], value2);};
				else
					calculators << [index, index2, opFunc](QList<double> values)
						{return opFunc(values[index], values[index2]);};
				i += 2;
			}
			else
			{
				// only single argument
				if (isNumber)
					calculators << [value](QList<double>) { return value; };
				else
					calculators << ([index](QList<double> values) { return values[index];});
			}
		}
	}
	return {calculators, variableNames};
}

Probes::ValueOperator Probes::operatorFromString(QString operatorString)
{
	if (operatorString == "+") return [](double a, double b) { return a + b; };
	else if (operatorString == "-") return [](double a, double b) { return a - b; };
	else if (operatorString == "*") return [](double a, double b) { return a * b; };
	else if (operatorString == "/") return [](double a, double b) { return a / b; };
	Q_ASSERT(false);
}

} /* namespace OODebug */
