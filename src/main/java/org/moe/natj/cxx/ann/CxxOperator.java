/*
Copyright 2014-2016 Intel Corporation

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

package org.moe.natj.cxx.ann;

import org.moe.natj.cxx.CxxMethodVirtuality;
import org.moe.natj.cxx.CxxOperatorKind;

import java.lang.annotation.ElementType;
import java.lang.annotation.Inherited;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Marks the non-static method as being a C++ operator.
 */
@Inherited
@Retention(RetentionPolicy.RUNTIME)
@Target({
        ElementType.METHOD
})
public @interface CxxOperator {
    /**
     * Operator kind.
     *
     * @return operator kind
     */
    CxxOperatorKind value();

    /**
     * The C++ virtuality of the method.
     *
     * @return virtuality of the method
     */
    CxxMethodVirtuality virtuality() default CxxMethodVirtuality.NON_VIRTUAL;

    /**
     * The C++ const qualifier of the method.
     *
     * @return C++ const qualifier of the method
     */
    boolean isConst() default false;
}
