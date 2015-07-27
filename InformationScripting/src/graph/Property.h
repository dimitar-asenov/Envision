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

#pragma once

#include "../informationscripting_api.h"

namespace InformationScripting {

	// Inspired by: http://channel9.msdn.com/Events/GoingNative/2013/Inheritance-Is-The-Base-Class-of-Evil
	class Property {
		public:
			Property() = default;
			template <class DataType>
			Property(DataType propertyData) : data_{std::make_shared<PropertyData<DataType>>(std::move(propertyData))} {}

			friend boost::python::object pythonObject(const Property& p) {
				return p.data_->pythonObject();
			}

			template <class ConvertTo>
			operator ConvertTo() const {
				if (auto propertyData = std::dynamic_pointer_cast<PropertyData<ConvertTo>>(data_))
					return propertyData->data_;
				throw new std::bad_cast;
			}

		private:
			struct PropertyDataConcept {
					virtual ~PropertyDataConcept() = default;
					virtual boost::python::object pythonObject() const = 0;
			};

			template <class DataType, class = void>
			struct PropertyData : PropertyDataConcept {
					PropertyData(DataType data) : data_{std::move(data)} {}

					virtual boost::python::object pythonObject() const override {
						return boost::python::object(data_);
					}
					DataType data_;
			};
			template <class DataType>
			struct PropertyData<DataType, typename std::enable_if<std::is_pointer<DataType>::value>::type>
					: PropertyDataConcept {
					PropertyData(DataType data) : data_{data} {}

					virtual boost::python::object pythonObject() const override {
						return boost::python::object(boost::python::ptr(data_));
					}
					DataType data_;
			};

			std::shared_ptr<PropertyDataConcept> data_;
	};
} /* namespace InformationScripting */
